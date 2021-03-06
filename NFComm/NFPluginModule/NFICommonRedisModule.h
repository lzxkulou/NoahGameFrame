// -------------------------------------------------------------------------
//    @FileName         :    NFICommonRedisModule.h
//    @Author           :    Chuanbo.Guo
//    @Date             :    2012-12-15
//    @Module           :    NFICommonRedisModule
//
// -------------------------------------------------------------------------

#ifndef NFI_COMMON_REDIS_MODULE_H
#define NFI_COMMON_REDIS_MODULE_H

#include "NFIModule.h"

class NFICommonRedisModule
    : public NFIModule
{
public:
	virtual std::string GetPropertyCacheKey(const NFGUID& self) = 0;
	virtual std::string GetRecordCacheKey(const NFGUID& self) = 0;
	virtual std::string GetAccountCacheKey(const std::string& strAccount) = 0;
	virtual std::string GetTileCacheKey(const int& nSceneID) = 0;

    virtual NF_SHARE_PTR<NFIPropertyManager> GetCachePropertyInfo(const NFGUID& self) = 0;
    virtual NF_SHARE_PTR<NFIRecordManager> GetCacheRecordInfo(const NFGUID& self) = 0;

	virtual bool GetCachePropertyListPB(const NFGUID& self, NFMsg::ObjectPropertyList& propertyList) = 0;
	virtual bool GetCacheRecordListPB(const NFGUID& self, NFMsg::ObjectRecordList& recordList) = 0;
	virtual bool GetStoragePropertyListPB(const NFGUID& self, NFMsg::ObjectPropertyList& propertyList) = 0;
	virtual bool GetStorageRecordListPB(const NFGUID& self, NFMsg::ObjectRecordList& recordList) = 0;

    virtual bool SetCachePropertyInfo(const NFGUID& self,  NF_SHARE_PTR<NFIPropertyManager> pPropertyManager) = 0;
    virtual bool SetCacheRecordInfo(const NFGUID& self, NF_SHARE_PTR<NFIRecordManager> pRecordManager) = 0;

	virtual bool SetStroragePropertyInfo(const NFGUID& self, NF_SHARE_PTR<NFIPropertyManager> pPropertyManager) = 0;
	virtual bool SetStrorageRecordInfo(const NFGUID& self, NF_SHARE_PTR<NFIRecordManager> pRecordManager) = 0;

    virtual bool RemoveCachePropertyInfo(const NFGUID& self) = 0;
    virtual bool RemoveCacheRecordInfo(const NFGUID& self) = 0;

    virtual NF_SHARE_PTR<NFIPropertyManager> NewPropertyManager(const NFGUID& self) = 0;
    virtual NF_SHARE_PTR<NFIRecordManager> NewRecordManager(const NFGUID& self) = 0;

    virtual bool ConvertPBToPropertyManager(const NFMsg::ObjectPropertyList& xMsg, NF_SHARE_PTR<NFIPropertyManager>& pPropertyManager, const bool bCache) = 0;
    virtual bool ConvertPBToRecordManager(const NFMsg::ObjectRecordList& xMsg, NF_SHARE_PTR<NFIRecordManager>& pRecordManager, const bool bCache) = 0;

    virtual bool ConvertPropertyManagerToPB(const NF_SHARE_PTR<NFIPropertyManager>& pPropertyManager, NFMsg::ObjectPropertyList& xMsg, const bool bCache) = 0;
    virtual bool ConvertRecordManagerToPB(const NF_SHARE_PTR<NFIRecordManager>& pRecordManager, NFMsg::ObjectRecordList& xMsg, const bool bCache) = 0;

};

#endif