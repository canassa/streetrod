"""Validate reconstructed data shapes against the real MSC compiler."""
import sys,struct,json
from pathlib import Path
sys.path.insert(0,'tools')
from sr.cdata import Types
from sr import dosbox
from sr.omf import Obj
t=Types(); names=sorted(t.defs)
checks=[]
for name in names:
 node=t.named(name);checks.append(('sizeof('+name+')',node['size']))
 for field,off,child in node['fields']:
  # offsetof() takes a member's address, which a bit-field has none of; struct Look
  # (bit-fields packed into one word) is the only such type, so only its sizeof is checked
  if name=='struct Look':continue
  checks.append(('offsetof('+name+', '+field+')',off))
out=Path('work/data-audit');out.mkdir(parents=True,exist_ok=True)
p=out/'SHAPES.C';p.write_text('#define offsetof(t,m) ((unsigned)&(((t *)0)->m))\n#include "STRUCTS.H"\nunsigned shapes[] = {\n'+',\n'.join(a for a,b in checks)+'\n};\n')
j=dosbox.Job(p,'/AM /Oas /Gs /FPi','shapes');dosbox.compile_all([j],name='shapes')
if not j.obj:raise SystemExit(j.log)
o=Obj(j.obj);seg=o.segments.index('_DATA')+1;data=o.data[seg]
values=struct.unpack('<'+'H'*len(checks),data[:len(checks)*2]);bad=[]
for (expr,want),got in zip(checks,values):
 if want!=got:bad.append((expr,want,got))
print(len(names),'types;',len(checks),'sizeof/offsetof checks;',len(bad),'mismatches',bad[:20])
(out/'type-check.json').write_text(json.dumps({'checks':len(checks),'bad':bad,'build':str(j.dir)},indent=2))

if bad:raise SystemExit(1)
