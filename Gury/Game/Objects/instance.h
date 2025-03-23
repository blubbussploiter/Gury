
#pragma once

#include <string>
#include <vector>

#include "../Gury/Game/Reflection/property.h"

#define RBX_CLONE_DEF(class)    \
	virtual class* clone() const \
	{							 \
		class* clone = new class(*this); \
		clone->setParent(parent); \
		return clone; \
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

		boost::signal<void(Instance*)> onChildAdded;
		boost::signal<void(Instance*)> onChildRemoved;

		boost::signal<void(Instance*)> onDescendentAdded;
		boost::signal<void(Instance*)> onDescendentRemoved;

		boost::signal<void(Instance*, std::string property)> onChanged;

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

		void setName(std::string newName) 
		{ 
			name = newName;
			onChanged(this, "Name");
		}
		std::string getName() { return name; }

		void setClassName2(std::string newClassName) { }
		void setClassName(std::string newClassName) { className = newClassName; }

		std::string getClassName() { return className; }

		bool getArchivable() { return archivable; }
		void setArchivable(bool narchivable) 
		{
			archivable = narchivable;
			onChanged(this, "archivable");
		}

		void remove();

		virtual void onRemove() {};
		virtual void onStep() {};

		bool contains(const RBX::Instance* child);

		void signalOnDescendentAdded(RBX::Instance* newParent, RBX::Instance* oldParent);
		void signalOnDescendentRemoved(RBX::Instance* newParent, RBX::Instance* oldParent);

		Instances* getChildren() { return children; }

		void setParent(Instance* instance);
		Instance* getParent();

		Instance* findFirstChild(std::string name);

		static void onInstanceUpdateStudioView(RBX::Instance* instance, std::string property);
		
		Instance() : name("Instance"), className("Instance"), parent(NULL), isParentLocked(false)
		{
			children = new RBX::Instances();
			archivable = true;
			onChanged.connect(onInstanceUpdateStudioView);
		}

		virtual ~Instance() { }

		RBX_CLONE_DEF(Instance);
		RTTR_ENABLE()
	};

	template <class Type>
	static bool IsA(RBX::Instance* i)
	{
		return (i && dynamic_cast<Type*>(i) != 0);
	}

	template <class Type>
	static Type* toInstance(Instance* i)
	{
		return dynamic_cast<Type*>(i);
	}

}
