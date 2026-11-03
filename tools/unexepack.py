import struct,sys
d=open(sys.argv[1],'rb').read()
h=struct.unpack('<14H',d[:28])
hdr=h[4]*16; img=bytearray(d[hdr:(h[2]-1)*512+h[1]])
cs=h[11]*16
ip,rcs,_,psize,sp,ss,dlen,sig=struct.unpack('<8H',img[cs:cs+16])
assert sig==0x4252
src=img[:cs]; out=bytearray(dlen*16); out[:len(src)]=src
si=len(src)-1
while src[si]==0xff: si-=1
di=len(out)-1
while True:
    cmd=src[si]; n=src[si-1]*256+src[si-2]; si-=3
    if cmd&0xfe==0xb0:
        b=src[si]; si-=1
        for _ in range(n): out[di]=b; di-=1
    elif cmd&0xfe==0xb2:
        for _ in range(n): out[di]=out[si]; di-=1; si-=1
    else: raise SystemExit('bad cmd %x'%cmd)
    if cmd&1: break
# relocs
p=cs+0x125; relocs=[]
for seg in range(16):
    c=struct.unpack('<H',img[p:p+2])[0]; p+=2
    for _ in range(c):
        relocs.append((struct.unpack('<H',img[p:p+2])[0],seg*0x1000)); p+=2
print('real CS:IP %04x:%04x SS:SP %04x:%04x image %d bytes relocs %d'%(rcs,ip,ss,sp,len(out),len(relocs)))
nh=0x20+((len(relocs)*4+15)//16)*16
nhp=(0x1c+len(relocs)*4+15)//16
tot=nhp*16+len(out)
H=struct.pack('<14H',0x5a4d,tot%512,(tot+511)//512,len(relocs),nhp,h[5],h[6],ss,sp,0,ip,rcs,0x1c,0)
rt=b''.join(struct.pack('<HH',o,s) for o,s in relocs)
o=(H+rt).ljust(nhp*16,b'\0')+out
open(sys.argv[2],'wb').write(o)
