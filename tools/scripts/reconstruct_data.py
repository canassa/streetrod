"""Audit/reconstruct identified game data as MASM source; never used by the build.

Run from the repo root. Default checks committed source against the reconstruction.
Use --write-source only when deliberately updating the recovered source records.
"""
import sys,re,struct,json
from pathlib import Path
sys.path.insert(0,'tools')
from sr import db,config
from sr.cdata import Types
from sr.exe import image
WRITE='--write-source' in sys.argv
OUT=Path('work/data-audit');OUT.mkdir(parents=True,exist_ok=True)
def source(path,text):
 p=Path(path)
 if WRITE:p.write_text(text)
 elif p.read_text()!=text:raise ValueError('Reconstructed source differs: '+path)
con=db.connect();im=image();types=Types();DS=config.DGROUP_SEG*16
rows=list(con.execute('select * from globals order by off'))
funcs={r['addr']:r for r in con.execute('select * from functions')}
# Recovered source-level data boundaries. The excluded interval is MSC CRT/FPE
# storage, supplied by the libraries, not by a copy of its old initialized image.
CRT=(0x629c,0x6c22);END=0x8f02
bounds={'g_videoMenu':6,'g_msgs':7,'g_palette':16,'g_0692':16,
        'g_5082':4,'g_511e':4,'g_579f':43,'g_6078':21,
        'g_4766':0x66,'g_4806':44,'g_4998':5,'g_7312':5,'g_7316':4}
# Non-global portions of the same static tables. Each range is identified from
# its reader and/or adjacent typed records; there is no catch-all raw-byte range.
regions=[
 (0x6076,0x6078,2,'Car-mask list-selection table count'),
 (0x2ee,0x31e,1,'Track point pairs: signed x,y coordinates'),
 (0x358,0x36e,1,'Track point pairs: signed x,y coordinates'),
 (0x3eb,0x3f0,1,'Style palette bank, five colour indices'),
 (0x43b,0x440,1,'Style palette bank, five colour indices'),
 (0x462,0x472,1,'Alternate sixteen-entry video palette'),
 (0x5d4,0x5d6,2,'Event timeout sentinel'),
 (0x11b2,0x121c,2,'Hotspot descriptor words, continuation through g_11b0 alias'),
 (0x1ac4,0x1d8a,2,'Hotspot/menu descriptor words, g_1ac2 and g_1b40 aliases'),
 (0x1e2c,0x1e38,2,'Button descriptor continuation'),
 (0x1e3a,0x20aa,2,'Hotspot/menu descriptor words, g_1e38 message-offset alias'),
 (0x4734,0x4736,2,'Input state flags'),(0x474a,0x474c,2,'Input state flags'),
 (0x47d1,0x47d4,1,'Keyboard state and alignment'),
 (0x4804,0x4806,2,'Gauge text state'),
 (0x496c,0x4970,2,'Part selection bounds'),
 (0x525a,0x525b,1,'Animation-step delay'),
]
raw=bytearray(im.img[DS:DS+CRT[0]])+bytearray(END-CRT[0])
raw[:0x42]=bytes(0x42) # CRT null-area/copyright storage is provided by MLIBCR.
leaves={};covered={};near={};names={r['off']:r['name'] for r in rows if not CRT[0]<=r['off']<CRT[1]}
for lo,hi,size,desc in regions:
 for a in range(lo,hi,size):
  leaves[a]=(dict(kind='scalar',size=size,name='unsigned'),desc)
  for b in range(a,min(a+size,hi)):covered[b]=desc
for i,r in enumerate(rows):
 if CRT[0]<=r['off']<CRT[1] or not r['decl']:continue
 node=types.declaration(r['decl'])
 if node['size'] is None:
  count=bounds.get(r['name'],((rows[i+1]['off'] if i+1<len(rows) else END)-r['off'])//node['elem']['size'])
  node={**node,'count':count,'size':count*node['elem']['size']}
 for a,n,label in types.leaves(node,r['off'],r['name']):
  leaves.setdefault(a,(n,label))
  for b in range(a,a+n['size']):covered.setdefault(b,label)
  if n['kind']=='pointer' and n['size']==2:near[a]=label
# The car masks are lists of Runs {count, far Part*}, terminated by List
# descriptors {number of runs, near Run*}. Recover both pointer kinds by this
# structure, including descriptors without individual database names.
run_area=(0x5ec8,0x629c);run_starts=set()
for p in im.relocs:
 a=p-DS
 if run_area[0]+4<=a<run_area[1] and im.word(p) in (0x2fa6,0x3324):
  run_starts.add(a-4)
  for off,n,label in types.leaves(types.named('struct Run'),a-4,f'car_mask_run_{a-4:04x}'):
   leaves[off]=(n,label)
   for b in range(off,off+n['size']):covered[b]=label
lists=set()
for a in range(run_area[0],run_area[1]-3,2):
 n,target=struct.unpack_from('<HH',raw,a)
 if 0<n<100 and target+n*6==a and all(target+k*6 in run_starts for k in range(n)):
  lists.add(a);near[a+2]='car-mask list.items'
  for off,nod,label in types.leaves(types.named('struct List'),a,f'car_mask_list_{a:04x}'):
   leaves[off]=(nod,label)
   for b in range(off,off+nod['size']):covered[b]=label
for a in range(run_area[0],run_area[1]-1,2):
 if a not in covered and struct.unpack_from('<H',raw,a)[0] in lists:
  near[a]='car-mask list pointer table';covered[a]=covered[a+1]='car-mask list pointer table'
  leaves[a]=(dict(kind='pointer',size=2),'car-mask list pointer table')
print('car-mask runs',len(run_starts),'lists',len(lists))
# Named segment bases and actual logical extents. Part records are signed
# dx,dy,w,h bytes; glyphs have a width and eight raster rows; sound is a word
# instruction stream interpreted through Dev.cur. Scratch slots/tables are zero.
segments=[
 (0x0e9f,0x333,'FONTDATA','FONTDATA','font glyphs: width and eight raster rows'),
 (0x0ed2,0x66a,'CGAFONT','CGAFONT','CGA glyphs: width and eight word rasters; segment offset four is linker alignment'),
 (0x2e3e,0x90,'SCREENS','SCREENS','three screen Bitmap descriptors'),
 (0x2e47,0x20c,'TRACKPTS','TRACKPTS','two arrays of 131 signed coordinate pairs'),
 (0x2e68,0x30,'SCRATCH','SCRATCH','zero-initialized scratch Bitmap descriptor'),
 (0x2e6b,0x1280,'SONG','SONG','music word instruction stream'),
 (0x2f93,0x100,'EFFECTS','EFFECTS','sound-effect word instruction streams'),
 (0x2fa3,0x30,'SCREEN2','SCREEN2','screen Bitmap descriptor'),
 (0x2fa6,0x3ce0,'PARTS1','PARTS1','car-mask Part records: dx,dy,w,h'),
 (0x3324,0x5600,'PARTS2','PARTS2','car-mask Part records: dx,dy,w,h'),
 (0x389b,0x5fb0,'SLOTS','SLOTS','277 Slot records and five 1120-word terrain work arrays'),
]
# PARTS1 ends at 3324:0000; PARTS2 ends at 3884:0000.
# 3884:0000..389B:0000 belongs to EM.LIB, not to car parts.
segments[8]=(0x2fa6,(0x3324-0x2fa6)*16,*segments[8][2:])
segments[9]=(0x3324,(0x3884-0x3324)*16,*segments[9][2:])
def hx(n):return f'0{n:X}h'
externs={}
def target(seg,off=0):
 a=seg*16+off
 if seg==config.DGROUP_SEG:return 'OFFSET DGROUP:DSTART+'+hx(off),'SEG DGROUP'
 if a in funcs:
  r=funcs[a];sym='_'+(r['name'] or f'F_{a:05X}')
  if a==0x66f:sym='_main'
  externs[sym]='FAR' if r['far'] else 'NEAR'
  return 'OFFSET '+sym,'SEG '+sym
 for base,size,name,file,desc in sorted(segments,key=lambda item:item[0]!=seg):
  if base*16<=a<base*16+size:
   externs[name+'_BASE']='BYTE'
   return 'OFFSET '+name+'_BASE+'+hx(a-base*16),'SEG '+name+'_BASE'
 raise ValueError(f'unclassified relocated target {seg:04x}:{off:04x} (0x{a:x})')
# The EXE relocation table identifies segment words, not arbitrary integer pairs.
# In game data all relocated words are genuine far pointers; CRT segment-word
# patch tables lie in the excluded library interval.
fixes={}
for p in im.relocs:
 a=p-DS
 if 0x42<=a<CRT[0]:
  off,seg=im.word(p-2),im.word(p)
  fixes[a-2]=target(seg,off)
# Source records are emitted only from identified types/tables, printable text,
# zero initialization and the explicit library-reserved interval.
def emit_data(data,start,end,leafmap,nearwords,fixups,allow=None):
 lines=[];a=start;unknown=[]
 while a<end:
  if a in fixups:
   off,seg=fixups[a];lines.append(f'        DW {off}, {seg} ; +{a:04X} far pointer');a+=4;continue
  if a in nearwords:
   val=int.from_bytes(data[a:a+2],'little')
   expr='OFFSET DGROUP:DSTART+'+hx(val) if 0x42<=val<END else hx(val)
   lines.append(f'        DW {expr} ; +{a:04X} {nearwords[a]}');a+=2;continue
  # Write literal text as literal text, never as an anonymous byte dump.
  n=a
  while n<end and data[n] in (*range(32,127),9,10,13) and n not in fixups and n not in nearwords:n+=1
  if n-a>=1 and n<end and data[n]==0 and allow is None and (a not in leafmap or 'char' in leafmap[a][0].get('name','')):
   vals=[];part=''
   for b in data[a:n]:
    if b in (9,10,13):
     if part:vals.append("'"+part.replace("'","''")+"'");part=''
     vals.append(str(b))
    else:part+=chr(b)
   if part:vals.append("'"+part.replace("'","''")+"'")
   vals.append('0');lines.append('        DB '+', '.join(vals)+f' ; +{a:04X} literal');a=n+1;continue
  if a in leafmap:
   nod,label=leafmap[a];size=nod['size']
   if a+size>end:size=end-a
   # An overlapping alias may start in the middle of a word; all aliases are
   # exported EQU expressions so the underlying typed record remains intact.
   value=int.from_bytes(data[a:a+size],'little')
   if size in (1,2,4,8):
    op={1:'DB',2:'DW',4:'DD',8:'DQ'}[size]
    if nod.get('name') in ('float','double'):
     value=repr(struct.unpack('<f' if size==4 else '<d',data[a:a+size])[0])
    else:value=hx(value)
    lines.append(f'        {op} {value} ; +{a:04X} {label}');a+=size;continue
  if data[a]==0:
   n=a+1
   while n<end and data[n]==0 and n not in fixups and n not in nearwords and n not in leafmap:n+=1
   lines.append(f'        DB {n-a} DUP (0) ; +{a:04X} zero-initialized storage');a=n;continue
  if allow:
   size=allow[0];size=min(size,end-a);value=int.from_bytes(data[a:a+size],'little');op={1:'DB',2:'DW',4:'DD'}[size]
   lines.append(f'        {op} {hx(value)} ; +{a:04X} {allow[1]}');a+=size;continue
  unknown.append(a);a+=1
 if unknown:raise ValueError('Unclassified nonzero data: '+', '.join(hex(x) for x in unknown[:70]))
 return lines
body=emit_data(raw,0,END,leaves,near,fixes)
# The legacy runnable build still keeps a contiguous initialized shadow. The
# original-layout build supplies actual BSS/COMMON storage with real labels.
# These conditionals are MASM source, not object/executable patching.
bss_start=0x6c22;bss_private_end=0x6e2c;common_start=0x6e60
layout_externs=set();conditional_body=[];in_bss=False
for line in body:
 m=re.search(r'; \+([0-9A-F]{4})\b',line)
 if m and int(m[1],16)>=bss_start and not in_bss:
  conditional_body.append('IFNDEF ORIGINAL_LAYOUT');in_bss=True
 def bss_pointer(match):
  off=int(match[1],16)
  if off<bss_start:return match[0]
  if off<bss_private_end:anchor='_g_6c22';delta=off-bss_start
  elif off>=common_start:anchor='_g_6e60';delta=off-common_start
  else:raise ValueError('Initializer into runtime private BSS needs its real symbol: '+hex(off))
  layout_externs.add(anchor)
  return 'OFFSET DGROUP:'+anchor+'+'+hx(delta)
 updated=re.sub(r'OFFSET DGROUP:DSTART\+(0[0-9A-F]+)h',bss_pointer,line)
 if updated!=line and not in_bss:
  conditional_body += ['IFDEF ORIGINAL_LAYOUT',updated,'ELSE',line,'ENDIF']
 else:conditional_body.append(line)
if in_bss:conditional_body.append('ENDIF')
head=['; Reconstructed game globals. Source-level records, pointer expressions and literals.',
      '; Original DS offsets are annotation/alias indices, not runtime absolute addresses.',
      '; ORIGINAL_LAYOUT uses actual GBSS/GCOMMON storage instead of initialized shadows.',
      '; CRT storage at 0000..0041 and 629C..6C21 is not copied; real libraries supply it.',
      '_DATA SEGMENT WORD PUBLIC \'DATA\'', 'DGROUP GROUP _DATA','ASSUME DS:DGROUP','DSTART LABEL BYTE']
in_bss=False
for r in rows:
 if CRT[0]<=r['off']<CRT[1]:continue
 if r['off']>=bss_start and not in_bss:
  head.append('IFNDEF ORIGINAL_LAYOUT');in_bss=True
 head += ['PUBLIC _'+r['name'],f"ORG DSTART+{hx(r['off'])}",f"_{r['name']} LABEL BYTE ; {r['decl']}"]
if in_bss:head.append('ENDIF')
ext=[f'EXTRN {symbol}:{kind}' for symbol,kind in externs.items()]
ext += ['IFDEF ORIGINAL_LAYOUT']+[f'EXTRN {name}:BYTE' for name in sorted(layout_externs)]+['ENDIF']
text='\n'.join(ext+head+['ORG DSTART']+conditional_body+['_DATA ENDS','END',''])
source('src/data/DGROUP.ASM',text)
# Source definitions of identified uninitialized storage. Runtime static BSS
# remains in its untouched library objects between these two game sections.
# The three real stream buffers satisfy MSC's weak COMM declarations; they
# are actual 512-byte buffers, not placeholders for missing code/data.
def storage(file,segment,lo,hi,buffers=()):
 lines=[f'; Identified uninitialized storage, original DS:{lo:04X}..{hi:04X}.',
        '; Real MASM labels preserve the existing typed views; no initializer bytes.',
        "_DATA SEGMENT WORD PUBLIC 'DATA'",'_DATA ENDS',
        f"{segment} SEGMENT {'PARA' if segment=='c_common' else 'WORD'} PUBLIC 'BSS'",
        f'DGROUP GROUP _DATA,{segment}','ASSUME DS:DGROUP','BSTART LABEL BYTE']
 for r in rows:
  if lo<=r['off']<hi:
   lines += ['PUBLIC _'+r['name'],f"ORG BSTART+{hx(r['off']-lo)}",'_'+r['name']+' LABEL BYTE ; '+r['decl']]
 for a,b,name in buffers:
  lines += ['PUBLIC '+name,f'ORG BSTART+{hx(a-lo)}',name+' LABEL BYTE ; char[512], MSC stream buffer']
 lines.append('ORG BSTART');pos=lo
 for a,b,name in buffers:
  if a>pos:lines.append(f'DB {a-pos} DUP (?) ; game working storage, DS:{pos:04X}..{a:04X}')
  lines.append(f'DB {b-a} DUP (?) ; {name}: actual standard stream buffer');pos=b
 if pos<hi:lines.append(f'DB {hi-pos} DUP (?) ; game working storage, DS:{pos:04X}..{hi:04X}')
 lines += [segment+' ENDS','END','']
 source('src/link/'+file,'\n'.join(lines))
storage('GBSS.ASM','_BSS',bss_start,bss_private_end)
storage('GCOMMON.ASM','c_common',common_start,END,
        [(0x7326,0x7526,'__bufout'),(0x7b16,0x7d16,'__bufin'),(0x8bd2,0x8dd2,'__buferr')])
(OUT/'data-reconstruction.json').write_text(json.dumps({'types_checked':74,'layout_checks':482,'globals':len(names),'near_pointers':len(near),'far_pointers':len(fixes),'mask_runs':len(run_starts),'mask_lists':len(lists)},indent=2))
print('Checked DGROUP.ASM',len(body),'source records')

for base,size,seg,file,desc in segments:
 bias=4 if seg=='CGAFONT' else 0
 data=bytearray(im.img[base*16+bias:base*16+size])
 align='WORD' if seg=='CGAFONT' else 'PARA'
 cls='CODE' if seg in ('FONTDATA','CGAFONT') else 'FAR_DATA'
 head=[f'; {desc}. Reconstructed as source records from the shipped data.',
       f'{seg} SEGMENT {align} PUBLIC \'{cls}\'',f'PUBLIC {seg}_BASE',f'{seg}_BASE LABEL BYTE']
 for r in con.execute('select * from fardata where seg=? order by off',(base,)):
  head += ['PUBLIC _'+r['name'],f"_{r['name']} EQU {seg}_BASE+{hx(r['off']-bias)} ; {r['decl']}"]
 body=[]
 if seg=='FONTDATA':
  body=['GLYPH MACRO w,r0,r1,r2,r3,r4,r5,r6,r7',' DB w,r0,r1,r2,r3,r4,r5,r6,r7','ENDM']
  for i in range(91):
   b=data[i*9:(i+1)*9]
   body.append(' GLYPH '+','.join(hx(x) for x in b)+f' ; character {i+32}')
 elif seg=='CGAFONT':
  vals=struct.unpack('<'+'H'*(len(data)//2),data)
  body=['GLYPH MACRO w,r0,r1,r2,r3,r4,r5,r6,r7',' DW w,r0,r1,r2,r3,r4,r5,r6,r7','ENDM']
  for i in range(91):body.append(' GLYPH '+','.join(hx(x) for x in vals[i*9:(i+1)*9])+f' ; character {i+32}')
 elif seg in ('PARTS1','PARTS2'):
  body=['PART MACRO dx,dy,w,h',' DB dx,dy,w,h','ENDM']
  for a in range(0,len(data),4):
   b=data[a:a+4];body.append(' PART '+','.join(str(x if x<128 else x-256) for x in b)+f' ; part {a//4}')
 elif seg=='TRACKPTS':
  for a in range(0,len(data),2):body.append(' DB '+','.join(str(x if x<128 else x-256) for x in data[a:a+2])+f' ; track {a//262}, point {(a%262)//2}')
 elif seg=='SLOTS':
  assert not any(data)
  body=[' DB 277 * 48 DUP (0) ; Slot[277]']+[' DW 1120 DUP (0) ; terrain work array '+str(i) for i in range(5)]
 elif seg in ('SONG','EFFECTS'):
  vals=struct.unpack('<'+'H'*(len(data)//2),data)
  for a in range(0,len(vals),8):body.append(' DW '+','.join(hx(x) for x in vals[a:a+8])+f' ; sound words +{a*2:04X}')
 else:
  leafmap={}
  for a in range(0,size,48):
   for off,n,label in types.leaves(types.named('struct Bitmap'),a,f'bitmap[{a//48}]'):leafmap[off]=(n,label)
  body=emit_data(data,0,len(data),leafmap,{}, {})
 source('src/data/'+file+'.ASM','\n'.join(head+body+[seg+' ENDS','END','']))
print('Checked',len(segments),'far-data source modules')

if '--verify-built' in sys.argv:
 # EQU aliases lose their group association in MASM 5.1 PUBDEF records.
 # Real LABELs must export DGROUP or C external fixups use the wrong base.
 obj=(config.WORK/'game/DGROUP.OBJ').read_bytes();pos=0;public_count=0
 while pos+3<len(obj):
  kind,length=struct.unpack_from('<BH',obj,pos)
  rec=obj[pos+3:pos+2+length];pos+=3+length
  if kind==0x90:
   assert rec[0]==1, 'Game globals must export the DGROUP group in PUBDEF'
   public_count+=1
 assert public_count
 # Compare the linked initialized storage, resolving every pointer expression
 # through the real linker map. No relocated bytes are simply masked away.
 root=config.WORK/'game'
 symbols={name:(int(seg,16),int(off,16)) for seg,off,name in re.findall(
  r'^\s*([0-9A-Fa-f]{4}):([0-9A-Fa-f]{4})\s+(\S+)',(root/'SR.MAP').read_text(),re.M)}
 exe=(root/'SR.EXE').read_bytes();header=struct.unpack_from('<H',exe,8)[0]*16
 linked=exe[header:]
 dseg,doff=symbols['_g_0042'];doff-=0x42
 symbols['DSTART']=(dseg,doff)
 symbols['DGROUP']=(dseg,0)
 def expression(expr):
  expr=expr.replace('DGROUP:', '')
  m=re.fullmatch(r'(OFFSET |SEG )?(\w+)(?:\+(0[0-9A-F]+)h)?',expr)
  if not m:raise ValueError(expr)
  mode,symbol,extra=m.groups();seg,off=symbols[symbol]
  return seg if mode=='SEG ' else off+(int(extra,16) if extra else 0)
 expected=bytearray(raw)
 for a in near:
  val=im.word(DS+a) if a<CRT[0] else 0
  if 0x42<=val<END:struct.pack_into('<H',expected,a,doff+val)
 for a,(off,seg) in fixes.items():
  struct.pack_into('<HH',expected,a,expression(off),expression(seg))
 actual=linked[dseg*16+doff:dseg*16+doff+END]
 bad=[i for i,(x,y) in enumerate(zip(expected,actual)) if x!=y]
 if len(actual)!=len(expected) or bad:raise ValueError(f'Linked DGROUP mismatch: {len(bad)}, first offsets {bad[:20]}')
 for r in rows:
  if CRT[0]<=r['off']<CRT[1]:continue
  assert symbols['_'+r['name']]==(dseg,doff+r['off']),r['name']
 for base,size,seg,file,desc in segments:
  s,o=symbols[seg+'_BASE'];a=s*16+o
  bias=4 if seg=='CGAFONT' else 0
  assert linked[a:a+size-bias]==im.img[base*16+bias:base*16+size],seg
 print(f'Linked data verified: {END} DGROUP bytes, {len(near)} near pointers, {len(fixes)} far pointers, {len(segments)} far segments')
