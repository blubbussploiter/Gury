
#pragma once

#include <string>
#include <vector>

#include "../Gury/Game/Reflection/property.h"
#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Lua/signaldescriptor.h"

/*
#define RBX_CLONE_DEF(class)    \
	virtual class* clone() const \
	{							 \
		class* clone = new class(*this); \
		clone->setParent(parent); \
		return clone; \
	} 
	*/

#define RBX_CLONE_DEF(class)    \
	virtual class* clone()       \
	{	\
		return instanceClone<class>((class*)this);	\
	}

namespace RBX
{

	class Instance;
	class GuidItem;

	typedef std::vector<Instance*> Instances;

	class Instance 
	{
	public:

		std::string name;
		std::string className;

		Instance* parent;
		Instances* children;

		bool archivable;

	public:

		GuidItem* id;

		boost::signal<void(Instance*, Instance*)> onChildAdded;
		boost::signal<void(Instance*, Instance*)> onChildRemoved;

		boost::signal<void(Instance*, Instance*)> onDescendentAdded;
		boost::signal<void(Instance*, Instance*)> onDescendentRemoved;

		boost::signal<void(Instance*, rttr::property)> onChanged;
		boost::signal<void(Instance*, Instance*)> onAncestorChanged;

		SignalDesc<void(Instance*, rttr::property)>* desc_onChanged;
		SignalDesc<void(Instance*, Instance*)>* desc_onAncestryChanged; /* this is never used, change that. */
		SignalDesc<void(Instance*, Instance*)>* desc_onChildAdded;
		SignalDesc<void(Instance*, Instance*)>* desc_onChildRemoved;
		SignalDesc<void(Instance*, Instance*)>* desc_onDescendentAdded;
		SignalDesc<void(Instance*, Instance*)>* desc_onDescendentRemoved;

		bool isParentLocked;
		bool isAncestorOf(RBX::Instance* i);

		template <typename T>
		T* findFirstAncestorWhichIsA(std::string className)
		{
			const RBX::Instance* v2; // eax

			v2 = parent;
			if (!parent)
				return 0;
			while (1)
			{
				v2 = v2->parent;
				if (v2 && v2->className == className)
					break;
				if (!v2)
					return 0;
			}
			return (T*)v2;
		}

		std::string getFullName()
		{
			Instance* parent = this->parent;
			std::string fullName;

			if (parent)
			{
				if (parent->parent)
				{
					fullName = parent->getFullName();
					fullName += ".";
					fullName += name;
				}
				else
				{
					fullName = name;
				}
				return fullName;
			}
			else
			{
				return name;
			}
		}

		template <typename T>
		inline T* findFirstChild(std::string name)
		{
			return (T*)findFirstChild(name);
		}

		template<typename T>
		inline T* findFirstChildOfClass(std::string name)
		{
			Instance* child;
			for (unsigned int i = 0; i < getChildren()->size(); i++)
			{
				child = getChildren()->at(i);
				if (child && child->getClassName() == name)
					return dynamic_cast<T*>(child);
			}
			return 0;
		}

		template <typename T>

		T* instanceClone(T* instance)
		{
			if (IsA<Instance>(instance)) {

				T* ourClone = new T();

				rttr::type global_type = rttr::detail::get_type_from_instance(instance);

				rttr::array_range<rttr::property> properties = global_type.get_properties();
				for (rttr::property property : properties)
				{
					rttr::variant ourValue = property.get_value(instance);
					if (property.get_name() != "Parent") {
						property.set_value(ourClone, ourValue);
					}
				}

				for (size_t i = 0; i < ourClone->children->size(); i++) {
					T* child = (T*)(ourClone->children->at(i)->clone());
					child->setParent(ourClone);
				}

				ourClone->setParent(0);

				return ourClone;

			}
		}

		void setName(std::string newName) 
		{ 
			name = newName;
			onChanged(this, getPropertyByName("Name"));
		}
		std::string getName() { return name; }

		void setClassName2(std::string newClassName) { }
		void setClassName(std::string newClassName) { className = newClassName; }

		std::string getClassName() { return className; }

		bool getArchivable() { return archivable; }
		void setArchivable(bool narchivable) 
		{
			archivable = narchivable;
			onChanged(this, getPropertyByName("archivable"));
		}

		void remove();

		virtual void onStep() {};

		bool contains(const RBX::Instance* child);

		void signalOnDescendentAdded(RBX::Instance* newParent, RBX::Instance* oldParent);
		void signalOnDescendentRemoved(RBX::Instance* newParent, RBX::Instance* oldParent);

		Instances* getChildren() { return children; }

		void setParent(Instance* instance);
		Instance* getParent();

		Instance* findFirstChild(std::string name);

		rttr::property getPropertyByName(std::string propertyName);

		static void onInstanceUpdateStudioView(RBX::Instance* instance, rttr::property property);
		
		Instance() : name("Instance"), className("Instance"), parent(NULL), isParentLocked(false)
		{
			children = new RBX::Instances();
			archivable = true;

			onChanged.connect(onInstanceUpdateStudioView);

			desc_onChanged = new SignalDesc<void(Instance*, rttr::property)>(&onChanged, "Changed");
			desc_onChildAdded = new SignalDesc<void(Instance*, Instance*)>(&onChildAdded, "ChildAdded");
			desc_onChildRemoved = new SignalDesc<void(Instance*, Instance*)>(&onChildRemoved, "ChildRemoved");
			desc_onDescendentAdded = new SignalDesc<void(Instance*, Instance*)>(&onDescendentAdded, "DescendentAdded");
			desc_onDescendentRemoved = new SignalDesc<void(Instance*, Instance*)>(&onDescendentRemoved, "DescendentRemoving");
			desc_onAncestryChanged = new SignalDesc<void(Instance*, Instance*)>(&onAncestorChanged, "AncestryChanged"); 
		}

		~Instance() {}

		RBX_CLONE_DEF(Instance)
		RTTR_ENABLE()
	};

	template <class TypeA, class TypeB>
	static bool typeCompareAny(TypeA* i)
	{
		return (toAny<TypeA, TypeB>(i) != 0);
	}

	template <class Type>
	static bool IsA(RBX::Instance* i)
	{
		return (i && dynamic_cast<Type*>(i) != 0);
	}

	template <class TypeA, class TypeB>
	static TypeB* toAny(TypeA* i) {
		return dynamic_cast<TypeB*>(i);
	}

	template <class Type>
	static Type* toInstance(Instance* i)
	{
		return dynamic_cast<Type*>(i);
	}

}
