"""MSC 5.1 data shapes for source reconstruction (16-bit int, two-byte packing).

The layout is checked against sizeof/offsetof emitted by the actual compiler before
it is used for initialized data. This is intentionally limited to STRUCTS.H syntax.
"""
import ast
import re
from . import config


def number(s):
    def walk(n):
        if isinstance(n, ast.Constant) and isinstance(n.value, int): return n.value
        if isinstance(n, ast.BinOp):
            a,b=walk(n.left),walk(n.right)
            if isinstance(n.op,ast.Add): return a+b
            if isinstance(n.op,ast.Sub): return a-b
            if isinstance(n.op,ast.Mult): return a*b
        raise ValueError(s)
    return walk(ast.parse(s.strip(), mode='eval').body)


def split_top(text, sep):
    level=0;start=0;out=[]
    for i,c in enumerate(text):
        if c in '({[':level+=1
        elif c in ')}]':level-=1
        elif c==sep and not level:out.append(text[start:i].strip());start=i+1
    if text[start:].strip():out.append(text[start:].strip())
    return out


def align(n,a):return (n+a-1)//a*a


class Types:
    def __init__(self):
        text=re.sub(r'/\*.*?\*/','', (config.INCLUDE/'STRUCTS.H').read_text(),flags=re.S)
        self.defs={};self.cache={};at=0
        pattern=re.compile(r'\b(struct|union)\s+(\w+)\s*\{')
        while m:=pattern.search(text,at):
            end=self.brace_end(text,m.end()-1)
            self.defs[m.group(1)+' '+m.group(2)]=(m.group(1),text[m.end():end])
            at=end+1

    @staticmethod
    def brace_end(text,start):
        depth=0
        for i in range(start,len(text)):
            if text[i]=='{':depth+=1
            if text[i]=='}':
                depth-=1
                if not depth:return i
        raise ValueError(text)

    def base(self,text):
        text=text.strip()
        anonymous=re.match(r'(struct|union)\s*\{',text)
        if anonymous:
            end=self.brace_end(text,text.index('{'))
            return self.record(anonymous.group(1),text[text.index('{')+1:end]),text[end+1:].strip()
        named=re.match(r'(struct|union)\s+(\w+)',text)
        if named:
            name=named.group(0)
            # Forward structures may only be used through pointers.
            node=lambda:dict(self.named(name),ctype=name)
            node.ctype=name
            return node,text[named.end():].strip()
        m=re.match(r'(?:(?:unsigned|signed)\s+)?(?:char|short|int|long|float|double|void)\b|unsigned\b|signed\b',text)
        if not m:raise ValueError('unsupported declaration: '+text)
        name=m.group(0);size=(1 if 'char' in name else 8 if name=='double' else 4 if name in ('long','unsigned long','signed long','float') else 0 if name=='void' else 2)
        return dict(kind='scalar',name=name,size=size,align=min(2,max(1,size)),ctype=name),text[m.end():].strip()

    def declarator(self,base,decl):
        # The identifier marker is @ in declarations and field names are
        # normalized to it below. Function pointers are far in the medium model.
        before,after=decl.split('@',1)
        if '*' in before:
            prefix=before[:before.rfind('*')]
            far=bool(re.search(r'\bfar\s*$',prefix)) or '(' in before and 'near' not in prefix
            basetext=base.ctype if callable(base) else base.get('ctype','')
            node=dict(kind='pointer',name=decl,size=4 if far else 2,align=2,far=far,
                      ctype=(basetext+' '+before.strip()).strip())
        else:
            node=base() if callable(base) else base
        dims=re.findall(r'\[([^]]*)\]',after)
        for dim in reversed(dims):
            count=number(dim) if dim.strip() else None
            node=dict(kind='array',elem=node,count=count,size=None if count is None else count*node['size'],align=node['align'],dalign=2)
        return node

    def declaration(self,text):
        base,decl=self.base(text)
        return self.declarator(base,decl)

    def named(self,name):
        if name not in self.cache:
            kind,body=self.defs[name];self.cache[name]=self.record(kind,body)
        return self.cache[name]

    def record(self,kind,body):
        fields=[];size=0;alignment=1;bitpos=0
        for statement in split_top(body,';'):
            base,tail=self.base(statement)
            for decl in split_top(tail,','):
                m=re.search(r'([A-Za-z_]\w*)\s*(?=\[|:|\)\s*\(|$)',decl)
                if not m:raise ValueError(statement)
                name=m.group(1);normalized=decl[:m.start()]+'@'+decl[m.end():]
                node=self.declarator(base,normalized)
                alignment=max(alignment,node['align'])
                if ':' in normalized:
                    bits=number(normalized.split(':')[1])
                    if bitpos==0 or bitpos+bits>16:
                        size=align(size,2);off=size;size+=2;bitpos=0
                    else:off=size-2
                    fields.append((name,off,node));bitpos=(bitpos+bits)%16
                    continue
                bitpos=0
                off=0 if kind=='union' else align(size,node['align'])
                fields.append((name,off,node));size=max(size,off+node['size'])
        return dict(kind=kind,size=align(size,alignment),align=alignment,fields=fields,dalign=2)

    def leaves(self,node,offset=0,name=''):
        if node['kind']=='array':
            if node['count'] is None:raise ValueError('array bound required: '+name)
            for i in range(node['count']):yield from self.leaves(node['elem'],offset+i*node['elem']['size'],f'{name}[{i}]')
        elif node['kind'] in ('struct','union'):
            fields=node['fields'][:1] if node['kind']=='union' else node['fields']
            for field,off,child in fields:yield from self.leaves(child,offset+off,name+'.'+field)
        else:yield offset,node,name
