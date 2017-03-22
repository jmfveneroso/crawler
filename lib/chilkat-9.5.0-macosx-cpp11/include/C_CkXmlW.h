// This is a generated source file for Chilkat version 9.5.0.66
#ifndef _C_CkXmlWH
#define _C_CkXmlWH
#include "chilkatDefs.h"

#include "Chilkat_C.h"


CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_Create(void);
CK_VISIBLE_PUBLIC void CkXmlW_Dispose(HCkXmlW handle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getCdata(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putCdata(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC void CkXmlW_getContent(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void  CkXmlW_putContent(HCkXmlW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_content(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC int CkXmlW_getContentInt(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putContentInt(HCkXmlW cHandle, int newVal);
CK_VISIBLE_PUBLIC void CkXmlW_getDebugLogFilePath(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void  CkXmlW_putDebugLogFilePath(HCkXmlW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_debugLogFilePath(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void CkXmlW_getDocType(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void  CkXmlW_putDocType(HCkXmlW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_docType(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getEmitBom(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putEmitBom(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getEmitCompact(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putEmitCompact(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getEmitXmlDecl(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putEmitXmlDecl(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC void CkXmlW_getEncoding(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void  CkXmlW_putEncoding(HCkXmlW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_encoding(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC int CkXmlW_getI(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putI(HCkXmlW cHandle, int newVal);
CK_VISIBLE_PUBLIC int CkXmlW_getJ(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putJ(HCkXmlW cHandle, int newVal);
CK_VISIBLE_PUBLIC int CkXmlW_getK(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putK(HCkXmlW cHandle, int newVal);
CK_VISIBLE_PUBLIC void CkXmlW_getLastErrorHtml(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_lastErrorHtml(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void CkXmlW_getLastErrorText(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_lastErrorText(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void CkXmlW_getLastErrorXml(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_lastErrorXml(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getLastMethodSuccess(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putLastMethodSuccess(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC int CkXmlW_getNumAttributes(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC int CkXmlW_getNumChildren(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getSortCaseInsensitive(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putSortCaseInsensitive(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getStandalone(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putStandalone(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC void CkXmlW_getTag(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void  CkXmlW_putTag(HCkXmlW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_tag(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC int CkXmlW_getTreeId(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_getVerboseLogging(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void  CkXmlW_putVerboseLogging(HCkXmlW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC void CkXmlW_getVersion(HCkXmlW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_version(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_AccumulateTagContent(HCkXmlW cHandle, const wchar_t *tag, const wchar_t *skipTags, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_accumulateTagContent(HCkXmlW cHandle, const wchar_t *tag, const wchar_t *skipTags);
CK_VISIBLE_PUBLIC BOOL CkXmlW_AddAttribute(HCkXmlW cHandle, const wchar_t *name, const wchar_t *value);
CK_VISIBLE_PUBLIC BOOL CkXmlW_AddAttributeInt(HCkXmlW cHandle, const wchar_t *name, int value);
CK_VISIBLE_PUBLIC BOOL CkXmlW_AddChildTree(HCkXmlW cHandle, HCkXmlW tree);
CK_VISIBLE_PUBLIC void CkXmlW_AddOrUpdateAttribute(HCkXmlW cHandle, const wchar_t *name, const wchar_t *value);
CK_VISIBLE_PUBLIC void CkXmlW_AddOrUpdateAttributeI(HCkXmlW cHandle, const wchar_t *name, int value);
CK_VISIBLE_PUBLIC void CkXmlW_AddStyleSheet(HCkXmlW cHandle, const wchar_t *styleSheet);
CK_VISIBLE_PUBLIC void CkXmlW_AddToAttribute(HCkXmlW cHandle, const wchar_t *name, int amount);
CK_VISIBLE_PUBLIC void CkXmlW_AddToChildContent(HCkXmlW cHandle, const wchar_t *tag, int amount);
CK_VISIBLE_PUBLIC void CkXmlW_AddToContent(HCkXmlW cHandle, int amount);
CK_VISIBLE_PUBLIC BOOL CkXmlW_AppendToContent(HCkXmlW cHandle, const wchar_t *str);
CK_VISIBLE_PUBLIC BOOL CkXmlW_BEncodeContent(HCkXmlW cHandle, const wchar_t *charset, HCkByteData inData);
CK_VISIBLE_PUBLIC BOOL CkXmlW_ChildContentMatches(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *pattern, BOOL caseSensitive);
CK_VISIBLE_PUBLIC BOOL CkXmlW_ChilkatPath(HCkXmlW cHandle, const wchar_t *pathCmd, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_chilkatPath(HCkXmlW cHandle, const wchar_t *pathCmd);
CK_VISIBLE_PUBLIC void CkXmlW_Clear(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_ContentMatches(HCkXmlW cHandle, const wchar_t *pattern, BOOL caseSensitive);
CK_VISIBLE_PUBLIC void CkXmlW_Copy(HCkXmlW cHandle, HCkXmlW node);
CK_VISIBLE_PUBLIC void CkXmlW_CopyRef(HCkXmlW cHandle, HCkXmlW copyFromNode);
CK_VISIBLE_PUBLIC BOOL CkXmlW_DecodeContent(HCkXmlW cHandle, HCkByteData outData);
CK_VISIBLE_PUBLIC BOOL CkXmlW_DecodeEntities(HCkXmlW cHandle, const wchar_t *str, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_decodeEntities(HCkXmlW cHandle, const wchar_t *str);
CK_VISIBLE_PUBLIC BOOL CkXmlW_DecryptContent(HCkXmlW cHandle, const wchar_t *password);
CK_VISIBLE_PUBLIC BOOL CkXmlW_EncryptContent(HCkXmlW cHandle, const wchar_t *password);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_ExtractChildByIndex(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_ExtractChildByName(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *attrName, const wchar_t *attrValue);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_FindChild(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC BOOL CkXmlW_FindChild2(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_FindNextRecord(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *contentPattern);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_FindOrAddNewChild(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_FirstChild(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_FirstChild2(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetAttributeName(HCkXmlW cHandle, int index, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getAttributeName(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetAttributeValue(HCkXmlW cHandle, int index, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getAttributeValue(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC int CkXmlW_GetAttributeValueInt(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetAttrValue(HCkXmlW cHandle, const wchar_t *name, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getAttrValue(HCkXmlW cHandle, const wchar_t *name);
CK_VISIBLE_PUBLIC int CkXmlW_GetAttrValueInt(HCkXmlW cHandle, const wchar_t *name);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetBinaryContent(HCkXmlW cHandle, BOOL unzipFlag, BOOL decryptFlag, const wchar_t *password, HCkByteData outData);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetChild(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetChild2(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetChildBoolValue(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetChildContent(HCkXmlW cHandle, const wchar_t *tagPath, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getChildContent(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetChildContentByIndex(HCkXmlW cHandle, int index, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getChildContentByIndex(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetChildExact(HCkXmlW cHandle, const wchar_t *tag, const wchar_t *content);
CK_VISIBLE_PUBLIC int CkXmlW_GetChildIntValue(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetChildTag(HCkXmlW cHandle, int index, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getChildTag(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetChildTagByIndex(HCkXmlW cHandle, int index, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getChildTagByIndex(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetChildWithAttr(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *attrName, const wchar_t *attrValue);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetChildWithContent(HCkXmlW cHandle, const wchar_t *content);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetChildWithTag(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetNthChildWithTag(HCkXmlW cHandle, const wchar_t *tag, int n);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetNthChildWithTag2(HCkXmlW cHandle, const wchar_t *tag, int n);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetParent(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetParent2(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetRoot(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void CkXmlW_GetRoot2(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_GetSelf(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetXml(HCkXmlW cHandle, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_getXml(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_GetXmlSb(HCkXmlW cHandle, HCkStringBuilderW sb);
CK_VISIBLE_PUBLIC BOOL CkXmlW_HasAttribute(HCkXmlW cHandle, const wchar_t *name);
CK_VISIBLE_PUBLIC BOOL CkXmlW_HasAttrWithValue(HCkXmlW cHandle, const wchar_t *name, const wchar_t *value);
CK_VISIBLE_PUBLIC BOOL CkXmlW_HasChildWithContent(HCkXmlW cHandle, const wchar_t *content);
CK_VISIBLE_PUBLIC BOOL CkXmlW_HasChildWithTag(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC BOOL CkXmlW_HasChildWithTagAndContent(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *content);
CK_VISIBLE_PUBLIC void CkXmlW_InsertChildTreeAfter(HCkXmlW cHandle, int index, HCkXmlW tree);
CK_VISIBLE_PUBLIC void CkXmlW_InsertChildTreeBefore(HCkXmlW cHandle, int index, HCkXmlW tree);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_LastChild(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_LastChild2(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_LoadSb(HCkXmlW cHandle, HCkStringBuilderW sb, BOOL autoTrim);
CK_VISIBLE_PUBLIC BOOL CkXmlW_LoadXml(HCkXmlW cHandle, const wchar_t *xmlData);
CK_VISIBLE_PUBLIC BOOL CkXmlW_LoadXml2(HCkXmlW cHandle, const wchar_t *xmlData, BOOL autoTrim);
CK_VISIBLE_PUBLIC BOOL CkXmlW_LoadXmlFile(HCkXmlW cHandle, const wchar_t *fileName);
CK_VISIBLE_PUBLIC BOOL CkXmlW_LoadXmlFile2(HCkXmlW cHandle, const wchar_t *fileName, BOOL autoTrim);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_NewChild(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *content);
CK_VISIBLE_PUBLIC void CkXmlW_NewChild2(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *content);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_NewChildAfter(HCkXmlW cHandle, int index, const wchar_t *tag, const wchar_t *content);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_NewChildBefore(HCkXmlW cHandle, int index, const wchar_t *tag, const wchar_t *content);
CK_VISIBLE_PUBLIC void CkXmlW_NewChildInt2(HCkXmlW cHandle, const wchar_t *tagPath, int value);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_NextSibling(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_NextSibling2(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC int CkXmlW_NumChildrenAt(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC int CkXmlW_NumChildrenHavingTag(HCkXmlW cHandle, const wchar_t *tag);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_PreviousSibling(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_PreviousSibling2(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_QEncodeContent(HCkXmlW cHandle, const wchar_t *charset, HCkByteData inData);
CK_VISIBLE_PUBLIC BOOL CkXmlW_RemoveAllAttributes(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC void CkXmlW_RemoveAllChildren(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_RemoveAttribute(HCkXmlW cHandle, const wchar_t *name);
CK_VISIBLE_PUBLIC void CkXmlW_RemoveChild(HCkXmlW cHandle, const wchar_t *tagPath);
CK_VISIBLE_PUBLIC void CkXmlW_RemoveChildByIndex(HCkXmlW cHandle, int index);
CK_VISIBLE_PUBLIC void CkXmlW_RemoveChildWithContent(HCkXmlW cHandle, const wchar_t *content);
CK_VISIBLE_PUBLIC void CkXmlW_RemoveFromTree(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SaveBinaryContent(HCkXmlW cHandle, const wchar_t *filename, BOOL unzipFlag, BOOL decryptFlag, const wchar_t *password);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SaveLastError(HCkXmlW cHandle, const wchar_t *path);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SaveXml(HCkXmlW cHandle, const wchar_t *fileName);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_SearchAllForContent(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *contentPattern);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SearchAllForContent2(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *contentPattern);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_SearchForAttribute(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *tag, const wchar_t *attr, const wchar_t *valuePattern);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SearchForAttribute2(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *tag, const wchar_t *attr, const wchar_t *valuePattern);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_SearchForContent(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *tag, const wchar_t *contentPattern);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SearchForContent2(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *tag, const wchar_t *contentPattern);
CK_VISIBLE_PUBLIC HCkXmlW CkXmlW_SearchForTag(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *tag);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SearchForTag2(HCkXmlW cHandle, HCkXmlW afterPtr, const wchar_t *tag);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SetBinaryContent(HCkXmlW cHandle, HCkByteData inData, BOOL zipFlag, BOOL encryptFlag, const wchar_t *password);
#if !defined(CHILKAT_MONO)
CK_VISIBLE_PUBLIC BOOL CkXmlW_SetBinaryContent2(HCkXmlW cHandle, const unsigned char *pByteData, unsigned long szByteData, BOOL zipFlag, BOOL encryptFlag, const wchar_t *password);
#endif
CK_VISIBLE_PUBLIC BOOL CkXmlW_SetBinaryContentFromFile(HCkXmlW cHandle, const wchar_t *filename, BOOL zipFlag, BOOL encryptFlag, const wchar_t *password);
CK_VISIBLE_PUBLIC void CkXmlW_SortByAttribute(HCkXmlW cHandle, const wchar_t *attrName, BOOL ascending);
CK_VISIBLE_PUBLIC void CkXmlW_SortByAttributeInt(HCkXmlW cHandle, const wchar_t *attrName, BOOL ascending);
CK_VISIBLE_PUBLIC void CkXmlW_SortByContent(HCkXmlW cHandle, BOOL ascending);
CK_VISIBLE_PUBLIC void CkXmlW_SortByTag(HCkXmlW cHandle, BOOL ascending);
CK_VISIBLE_PUBLIC void CkXmlW_SortRecordsByAttribute(HCkXmlW cHandle, const wchar_t *sortTag, const wchar_t *attrName, BOOL ascending);
CK_VISIBLE_PUBLIC void CkXmlW_SortRecordsByContent(HCkXmlW cHandle, const wchar_t *sortTag, BOOL ascending);
CK_VISIBLE_PUBLIC void CkXmlW_SortRecordsByContentInt(HCkXmlW cHandle, const wchar_t *sortTag, BOOL ascending);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SwapNode(HCkXmlW cHandle, HCkXmlW node);
CK_VISIBLE_PUBLIC BOOL CkXmlW_SwapTree(HCkXmlW cHandle, HCkXmlW tree);
CK_VISIBLE_PUBLIC BOOL CkXmlW_TagContent(HCkXmlW cHandle, const wchar_t *tagName, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkXmlW_tagContent(HCkXmlW cHandle, const wchar_t *tagName);
CK_VISIBLE_PUBLIC BOOL CkXmlW_TagEquals(HCkXmlW cHandle, const wchar_t *tag);
CK_VISIBLE_PUBLIC BOOL CkXmlW_UnzipContent(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_UnzipTree(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_UpdateAt(HCkXmlW cHandle, const wchar_t *tagPath, BOOL autoCreate, const wchar_t *value);
CK_VISIBLE_PUBLIC BOOL CkXmlW_UpdateAttrAt(HCkXmlW cHandle, const wchar_t *tagPath, BOOL autoCreate, const wchar_t *attrName, const wchar_t *attrValue);
CK_VISIBLE_PUBLIC BOOL CkXmlW_UpdateAttribute(HCkXmlW cHandle, const wchar_t *attrName, const wchar_t *attrValue);
CK_VISIBLE_PUBLIC BOOL CkXmlW_UpdateAttributeInt(HCkXmlW cHandle, const wchar_t *attrName, int value);
CK_VISIBLE_PUBLIC void CkXmlW_UpdateChildContent(HCkXmlW cHandle, const wchar_t *tagPath, const wchar_t *value);
CK_VISIBLE_PUBLIC void CkXmlW_UpdateChildContentInt(HCkXmlW cHandle, const wchar_t *tagPath, int value);
CK_VISIBLE_PUBLIC BOOL CkXmlW_ZipContent(HCkXmlW cHandle);
CK_VISIBLE_PUBLIC BOOL CkXmlW_ZipTree(HCkXmlW cHandle);
#endif
