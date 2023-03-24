/*
 * wasm-com.h
 * 
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * constant definitions for WebAssembly Component Model specification
 */
#ifndef __MLANG_WASM_COM_H__
#define __MLANG_WASM_COM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
Component Definitions
component ::= <preamble> s*:<section>*            => (component flatten(s*))
preamble  ::= <magic> <version> <layer>
magic     ::= 0x00 0x61 0x73 0x6D
version   ::= 0x0a 0x00
layer     ::= 0x01 0x00
section   ::=    section_0(<core:custom>)         => ϵ
            | m:section_1(<core:module>)          => [core-prefix(m)]
            | i*:section_2(vec(<core:instance>))  => core-prefix(i)*
            | t*:section_3(vec(<core:type>))      => core-prefix(t)*
            | c: section_4(<component>)           => [c]
            | i*:section_5(vec(<instance>))       => i*
            | a*:section_6(vec(<alias>))          => a*
            | t*:section_7(vec(<type>))           => t*
            | c*:section_8(vec(<canon>))          => c*
            | s: section_9(<start>)               => [s]
            | i*:section_10(vec(<import>))        => i*
            | e*:section_11(vec(<export>))        => e*
*/

/*
Instance Definitions
core:instance       ::= ie:<core:instanceexpr>                             => (instance ie)
core:instanceexpr   ::= 0x00 m:<moduleidx> arg*:vec(<core:instantiatearg>) => (instantiate m arg*)
                      | 0x01 e*:vec(<core:inlineexport>)                   => e*
core:instantiatearg ::= n:<core:name> 0x12 i:<instanceidx>                 => (with n (instance i))
core:sortidx        ::= sort:<core:sort> idx:<u32>                         => (sort idx)
core:sort           ::= 0x00                                               => func
                      | 0x01                                               => table
                      | 0x02                                               => memory
                      | 0x03                                               => global
                      | 0x10                                               => type
                      | 0x11                                               => module
                      | 0x12                                               => instance
core:inlineexport   ::= n:<core:name> si:<core:sortidx>                    => (export n si)

instance            ::= ie:<instanceexpr>                                  => (instance ie)
instanceexpr        ::= 0x00 c:<componentidx> arg*:vec(<instantiatearg>)   => (instantiate c arg*)
                      | 0x01 e*:vec(<inlineexport>)                        => e*
instantiatearg      ::= n:<name> si:<sortidx>                              => (with n si)
sortidx             ::= sort:<sort> idx:<u32>                              => (sort idx)
sort                ::= 0x00 cs:<core:sort>                                => core cs
                      | 0x01                                               => func
                      | 0x02                                               => value
                      | 0x03                                               => type
                      | 0x04                                               => component
                      | 0x05                                               => instance
inlineexport        ::= n:<name> si:<sortidx>                              => (export n si)
name                ::= len:<u32> n:<name-chars>                           => n (if len = |n|)
name-chars          ::= l:<label>                                          => l
                      | '[constructor]' r:<label>                          => [constructor]r
                      | '[method]' r:<label> '.' m:<label>                 => [method]r.m
                      | '[static]' r:<label> '.' s:<label>                 => [static]r.s
label               ::= w:<word>                                           => w
                      | l:<label> '-' w:<word>                             => l-w
word                ::= w:[0x61-0x7a] x*:[0x30-0x39,0x61-0x7a]*            => char(w)char(x)*
                      | W:[0x41-0x5a] X*:[0x30-0x39,0x41-0x5a]*            => char(W)char(X)*
*/
enum WasmCoreInstance {
    WasmCoreInstanceInstantiate = 0x00,
    WasmCoreInstanceInlineExport = 0x01,
};

#define WasmCoreInstantiateArg 0x12

enum WasmCoreSort {
    WasmCoreSortFunc = 0x00,
    WasmCoreSortTable = 0x01,
    WasmCoreSortMemory = 0x02,
    WasmCoreSortGlobal = 0x03,
    WasmCoreSortType = 0x10,
    WasmCoreSortModule = 0x11,
    WasmCoreSortInstance = 0x12,
};

enum WasmComInstance {
    WasmComInstanceInstantiate = 0x00,
    WasmComInstanceInlineExport = 0x01,
};

enum WasmComSort {
    WasmComSortCoreSort = 0x00,
    WasmComSortFun = 0x01,
    WasmComSortValue = 0x02,
    WasmComSortType = 0x03,
    WasmComSortComponent = 0x04,
    WasmComSortInstance = 0x05,
};

/*
Alias Definitions
alias       ::= s:<sort> t:<aliastarget>                => (alias t (s))
aliastarget ::= 0x00 i:<instanceidx> n:<name>           => export i n
              | 0x01 i:<core:instanceidx> n:<core:name> => core export i n
              | 0x02 ct:<u32> idx:<u32>                 => outer ct idx
*/
enum WasmComAliasTarget {
    WasmComAliasTargetComExport = 0x00,
    WasmComAliasTargetCoreExport = 0x01,
    WasmComAliasTargetOuter = 0x02,
};

/*
Type Definitions
core:type        ::= dt:<core:deftype>                  => (type dt)        (GC proposal)
core:deftype     ::= ft:<core:functype>                 => ft               (WebAssembly 1.0)
                   | st:<core:structtype>               => st               (GC proposal)
                   | at:<core:arraytype>                => at               (GC proposal)
                   | mt:<core:moduletype>               => mt
core:moduletype  ::= 0x50 md*:vec(<core:moduledecl>)    => (module md*)
core:moduledecl  ::= 0x00 i:<core:import>               => i
                   | 0x01 t:<core:type>                 => t
                   | 0x02 a:<core:alias>                => a
                   | 0x03 e:<core:exportdecl>           => e
core:alias       ::= s:<core:sort> t:<core:aliastarget> => (alias t (s))
core:aliastarget ::= 0x01 ct:<u32> idx:<u32>            => outer ct idx
core:importdecl  ::= i:<core:import>                    => i
core:exportdecl  ::= n:<core:name> d:<core:importdesc>  => (export n d)
*/
#define WasmCoreModuleType 0x50

enum WasmCoreModuleDecl {
    WasmCoreModuleDeclImport = 0x00,
    WasmCoreModuleDeclType = 0x01,
    WasmCoreModuleDeclAlias = 0x02,
    WasmCoreModuleDeclExport = 0x03,
};

#define WasmCoreModuleDeclAliasTarget 0x01

/*
type          ::= dt:<deftype>                            => (type dt)
deftype       ::= dvt:<defvaltype>                        => dvt
                | ft:<functype>                           => ft
                | ct:<componenttype>                      => ct
                | it:<instancetype>                       => it
primvaltype   ::= 0x7f                                    => bool
                | 0x7e                                    => s8
                | 0x7d                                    => u8
                | 0x7c                                    => s16
                | 0x7b                                    => u16
                | 0x7a                                    => s32
                | 0x79                                    => u32
                | 0x78                                    => s64
                | 0x77                                    => u64
                | 0x76                                    => float32
                | 0x75                                    => float64
                | 0x74                                    => char
                | 0x73                                    => string
defvaltype    ::= pvt:<primvaltype>                       => pvt
                | 0x72 lt*:vec(<labelvaltype>)            => (struct (field lt)*)
                | 0x71 case*:vec(<case>)                  => (variant case*)
                | 0x70 t:<valtype>                        => (list t)
                | 0x6f t*:vec(<valtype>)                  => (tuple t*)
                | 0x6e l*:vec(<label>)                    => (flags l*)
                | 0x6d l*:vec(<label>)                    => (enum l*)
                | 0x6c t*:vec(<valtype>)                  => (union t*)
                | 0x6b t:<valtype>                        => (option t)
                | 0x6a t?:<valtype>? u?:<valtype>?        => (result t? (error u)?)
                | 0x69 i:<typeidx>                        => (own i)
                | 0x68 i:<typeidx>                        => (borrow i)
labelvaltype  ::= l:<label> t:<valtype>                   => l t
case          ::= l:<label> t?:<valtype>? r?:<u32>?       => (case l t? (refines case-label[r])?)
<T>?          ::= 0x00                                    =>
                | 0x01 t:<T>                              => t
valtype       ::= i:<typeidx>                             => i
                | pvt:<primvaltype>                       => pvt
resourcetype  ::= 0x3f 0x7f f?:<funcidx>?                 => (resource (rep i32) (dtor f)?)
functype      ::= 0x40 ps:<paramlist> rs:<resultlist>     => (func ps rs)
paramlist     ::= lt*:vec(<labelvaltype>)                 => (param lt)*
resultlist    ::= 0x00 t:<valtype>                        => (result t)
                | 0x01 lt*:vec(<labelvaltype>)            => (result lt)*
componenttype ::= 0x41 cd*:vec(<componentdecl>)           => (component cd*)
instancetype  ::= 0x42 id*:vec(<instancedecl>)            => (instance id*)
componentdecl ::= 0x03 id:<importdecl>                    => id
                | id:<instancedecl>                       => id
instancedecl  ::= 0x00 t:<core:type>                      => t
                | 0x01 t:<type>                           => t
                | 0x02 a:<alias>                          => a
                | 0x04 ed:<exportdecl>                    => ed
importdecl    ::= en:<externname> ed:<externdesc>         => (import en ed)
exportdecl    ::= en:<externname> ed:<externdesc>         => (export en ed)
externdesc    ::= 0x00 0x11 i:<core:typeidx>              => (core module (type i))
                | 0x01 i:<typeidx>                        => (func (type i))
                | 0x02 t:<valtype>                        => (value t)
                | 0x03 b:<typebound>                      => (type b)
                | 0x04 i:<typeidx>                        => (component (type i))
                | 0x05 i:<typeidx>                        => (instance (type i))
typebound     ::= 0x00 i:<typeidx>                        => (eq i)
                | 0x01                                    => (sub resource)
*/

enum WasmComType {
    //primvaltype   
    WasmComTypeBool = 0x7F,
    WasmComTypeS8 = 0x7E,
    WasmComTypeU8 = 0x7D,
    WasmComTypeS16 = 0x7C,
    WasmComTypeU16 = 0x7B,
    WasmComTypeS32 = 0x7A,
    WasmComTypeU32 = 0x79,
    WasmComTypeS64 = 0x78,
    WasmComTypeU64 = 0x77,
    WasmComTypeF32 = 0x76,
    WasmComTypeF64 = 0x75,
    WasmComTypeChar = 0x74,
    WasmComTypeString = 0x73,
    //defvaltype
    WasmComTypeRecord = 0x72,
    WasmComTypeVariant = 0x71,
    WasmComTypeList = 0x70,
    WasmComTypeTuple = 0x6F,
    WasmComTypeFlags = 0x6E,
    WasmComTypeEnum = 0x6D,
    WasmComTypeUnion = 0x6C,
    WasmComTypeOption = 0x6B,
    WasmComTypeResult = 0x6A,
    WasmComTypeOwn = 0x69,
    WasmComTypeBorrow = 0x68,
};

enum WasmComOptional {
    WasmComOptionalAbsent = 0x00,
    WasmComOptionalPresent = 0x01
};

#define WasmComResourceType  0x7F3F //0x3F 0x7F, litttle endian
#define WasmComFuncType 0x40

enum WasmComResultList {
    WasmComResultListValue = 0x00,
    WasmComResultListLabelValue = 0x01,
};

#define WasmComComponentType 0x41
#define WasmComInstanceType 0x42
#define WasmComComponentDecl 0x03

enum WasmComInstanceDecl {
    WasmComInstanceDeclCoreType = 0x00,
    WasmComInstanceDeclType = 0x01,
    WasmComInstanceDeclAlias = 0x02,
    WasmComInstanceDeclExport = 0x04,
};

enum WasmComExternDesc {
    WasmComExternDescCoreModule = 0x1100, //0x00 0x11
    WasmComExternDescFunc = 0x01,
    WasmComExternDescValue = 0x02,
    WasmComExternDescType = 0x03,
    WasmComExternDescComponent = 0x04,
    WasmComExternDescInstance = 0x05,
};

enum WasmComTypeBound {
    WasmComTypeBoundEq = 0x00,
    WasmComTypeBoundSub = 0x01
};

/*
Canonical Definitions
canon    ::= 0x00 0x00 f:<core:funcidx> opts:<opts> ft:<typeidx> => (canon lift f opts type-index-space[ft])
           | 0x01 0x00 f:<funcidx> opts:<opts>                   => (canon lower f opts (core func))
           | 0x02 t:<typeidx>                                    => (canon resource.new t (core func))
           | 0x03 t:<valtype>                                    => (canon resource.drop t (core func))
           | 0x04 t:<typeidx>                                    => (canon resource.rep t (core func))
opts     ::= opt*:vec(<canonopt>)                                => opt*
canonopt ::= 0x00                                                => string-encoding=utf8
           | 0x01                                                => string-encoding=utf16
           | 0x02                                                => string-encoding=latin1+utf16
           | 0x03 m:<core:memidx>                                => (memory m)
           | 0x04 f:<core:funcidx>                               => (realloc f)
           | 0x05 f:<core:funcidx>                               => (post-return f)
*/
enum WasmComCanon {
    WasmComCanonLift = 0x0000, //0x00 0x00
    WasmComCanonLower = 0x0001, //0x01 0x00
    WasmComCanonResourceNew = 0x02,
    WasmComCanonResourceDrop = 0x03,
    WasmComCanonResourceRep = 0x04,
};

enum WasmComCanonOpt {
    WasmComCanonOptUtf8 = 0x00,
    WasmComCanonOptUtf16 = 0x01,
    WasmComCanonOptLU = 0x02,
    WasmComCanonOptMemory = 0x03,
    WasmComCanonOptRealloc = 0x04,
    WasmComCanonOptPostReturn = 0x05,
};

/*
Start Definitions
start ::= f:<funcidx> arg*:vec(<valueidx>) r:<u32> => (start f (value arg)* (result (value))ʳ)
*/

/*
Import and Export Definitions
import      ::= en:<externname> ed:<externdesc>                => (import en ed)
export      ::= en:<externname> si:<sortidx> ed?:<externdesc>? => (export en si ed?)
externname  ::= n:<name> ea:<externattrs>                      => n ea
externattrs ::= 0x00                                           => ϵ
              | 0x01 url:<URL>                                 => (id url)
URL         ::= b*:vec(byte)                                   => char(b)*, if char(b)* parses as a URL
*/
enum WasmComExternAttr {
    WasmComExternAttrEmpty = 0x00,
    WasmComExternAttrUrl = 0x01,
};

/*
Name Section
namesec    ::= section_0(namedata)
namedata   ::= n:<name>                (if n = 'component-name')
               name:<componentnamesubsec>?
               sortnames*:<sortnamesubsec>*
namesubsection_N(B) ::= N:<byte> size:<u32> B     (if size == |B|)

componentnamesubsec ::= namesubsection_0(<name>)
sortnamesubsec ::= namesubsection_1(<sortnames>)
sortnames ::= sort:<sort> names:<namemap>

namemap ::= names:vec(<nameassoc>)
nameassoc ::= idx:<u32> name:<name>
*/
#ifdef __cplusplus
}
#endif

#endif //__MLANG_WASM_COM_H__
