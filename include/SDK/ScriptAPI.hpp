#pragma once

namespace Universe::Scripting
{
    namespace API
    {
        class IVM;
        class IObject;
        class ICallbackInfo;

        using FunctionCallback = Function<void(ICallbackInfo& info)>;

        class IValue {
        public:
            virtual bool IsUndefined() = 0;
            virtual bool IsNull()      = 0;
            virtual bool IsString()    = 0;
            virtual bool IsFunction()  = 0;
            virtual bool IsObject()    = 0;
            virtual bool IsBoolean()   = 0;
            virtual bool IsNumber()    = 0;
            virtual bool IsExternal()  = 0;

            virtual String   ToString()   = 0;
            virtual bool     ToBoolean()  = 0;
            virtual double   ToNumber()   = 0;
            virtual void*    ToExternal() = 0;
            virtual IObject& ToObject()   = 0;
        };

        class IObject : public IValue {
        public:
            virtual void* GetInternal() = 0;

            virtual IValue& Get(String k) = 0;

            virtual void Set(int k, double v) = 0;
            virtual void Set(int k, int v)    = 0;
            virtual void Set(int k, bool v)   = 0;
            virtual void Set(int k, String v) = 0;

            virtual void Set(String k, double v) = 0;
            virtual void Set(String k, int v)    = 0;
            virtual void Set(String k, bool v)   = 0;
            virtual void Set(String k, String v) = 0;

            virtual void SetFunction(String k, FunctionCallback callback) = 0;
        };

        class IReturnValue {
        public:
            virtual void Set(String text) = 0;
            virtual void Set(double v)    = 0;
            virtual void Set(int v)       = 0;
            virtual void Set(bool v)      = 0;
            virtual void Set(void* v)     = 0;
            virtual void Set(IObject& o)  = 0;

            virtual void SetNull() = 0;
        };

        class ICallbackInfo {
        public:
            virtual IValue& operator[](int i) = 0;

            virtual IVM*          GetVM()          = 0;
            virtual int           Length()         = 0;
            virtual IReturnValue& GetReturnValue() = 0;
            virtual IObject&      This()           = 0;
        };

        class IGlobal {
        public:
            virtual void Set(String k, double v) = 0;
            virtual void Set(String k, int v)    = 0;
            virtual void Set(String k, bool v)   = 0;
            virtual void Set(String k, String v) = 0;
        };

        class IVM {
        public:
            virtual IGlobal& Global() = 0;

            virtual void ThrowException(String text) = 0;

            /* objects */
            virtual IObject& ObjectValue(String name, void* ptr) = 0;

            /* funcs */
            virtual void RegisterGlobalFunction(String name, FunctionCallback callback) = 0;
        };
    }
}