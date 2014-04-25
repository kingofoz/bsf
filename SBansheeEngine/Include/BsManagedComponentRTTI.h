#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObjectRTTI.h"
#include "BsManagedComponent.h"
#include "BsMonoManager.h"
#include "BsManagedSerializableObject.h"
#include "CmGameObjectManager.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponentRTTI : public RTTIType<ManagedComponent, Component, ManagedComponentRTTI>
	{
	private:
		String& getNamespace(ManagedComponent* obj)
		{
			return obj->mNamespace;
		}

		void setNamespace(ManagedComponent* obj, String& val)
		{
			obj->mNamespace = val;
		}

		String& getTypename(ManagedComponent* obj)
		{
			return obj->mTypeName;
		}

		void setTypename(ManagedComponent* obj, String& val)
		{
			obj->mTypeName = val;
		}

		ManagedSerializableObjectPtr getObjectData(ManagedComponent* obj)
		{
			return boost::any_cast<ManagedSerializableObjectPtr>(obj->mRTTIData);
		}

		void setObjectData(ManagedComponent* obj, ManagedSerializableObjectPtr val)
		{
			obj->mRTTIData = val;
		}

	public:
		ManagedComponentRTTI()
		{
			addPlainField("mNamespace", 0, &ManagedComponentRTTI::getNamespace, &ManagedComponentRTTI::setNamespace);
			addPlainField("mTypename", 1, &ManagedComponentRTTI::getTypename, &ManagedComponentRTTI::setTypename);
			addReflectablePtrField("mObjectData", 2, &ManagedComponentRTTI::getObjectData, &ManagedComponentRTTI::setObjectData);
		}

		void onSerializationStarted(IReflectable* obj)
		{
			ManagedComponent* mc = static_cast<ManagedComponent*>(obj);

			mc->mRTTIData = ManagedSerializableObject::create(mc->getManagedInstance());
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedComponent* mc = static_cast<ManagedComponent*>(obj);

			GameObjectManager::instance().registerOnDeserializationEndCallback(std::bind(&ManagedComponentRTTI::finalizeDeserialization, mc));
		}

		static void finalizeDeserialization(ManagedComponent* mc)
		{
			ManagedSerializableObjectPtr serializableObject = boost::any_cast<ManagedSerializableObjectPtr>(mc->mRTTIData);

			::MonoClass* monoClass = mono_object_get_class(serializableObject->getManagedInstance());
			MonoType* monoType = mono_class_get_type(monoClass);
			MonoReflectionType* runtimeType = mono_type_get_object(MonoManager::instance().getDomain(), monoType);

			mc->construct(serializableObject->getManagedInstance(), runtimeType);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ManagedComponent";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ManagedComponent;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<ManagedComponent>();
		}
	};
}