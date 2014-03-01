#ifndef NODEFTDI_HPP
#define NODEFTDI_HPP

#include <node.h>
#include <ftdi.h>

#define FTDI_VID 0x0403
#define FTDI_PID 0x6001

namespace node_ftdi {

class NodeFtdi : node::ObjectWrap {

    public:
        static void Initialize(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);

    protected:

        NodeFtdi() {};
        ~NodeFtdi() {};

        static v8::Persistent<v8::FunctionTemplate> constructor_template;

        static v8::Handle<v8::Value> Init(const v8::Arguments& args);
        static v8::Handle<v8::Value> Open(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetPins(const v8::Arguments& args);
        static v8::Handle<v8::Value> SetPins(const v8::Arguments& args);
        static v8::Handle<v8::Value> Close(const v8::Arguments& args);
        static v8::Handle<v8::Value> FindAll(const v8::Arguments& args);

    private:
        static bool initialized;
        static bool opened;
};

}

#endif