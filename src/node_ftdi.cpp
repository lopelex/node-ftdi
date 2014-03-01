#include <iostream>

#include "node_ftdi.hpp"

using namespace std;
using namespace v8;
using namespace node;
using namespace node_ftdi;

struct ftdi_context ftdic;

bool NodeFtdi::initialized = false;
bool NodeFtdi::opened = false;

const char* ToCString(Local<String> val) { return *String::Utf8Value(val); }

Persistent<FunctionTemplate> NodeFtdi::constructor_template;

void NodeFtdi::Initialize(Handle<Object> exports, Handle<Object> module) {

    Local<FunctionTemplate> t = FunctionTemplate::New();
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("Ftdi"));

    NODE_SET_METHOD(constructor_template, "init", NodeFtdi::Init);
    NODE_SET_METHOD(constructor_template, "open", NodeFtdi::Open);
    NODE_SET_METHOD(constructor_template, "getPins", NodeFtdi::GetPins);
    NODE_SET_METHOD(constructor_template, "setPins", NodeFtdi::SetPins);
    NODE_SET_METHOD(constructor_template, "close", NodeFtdi::Close);

    NODE_SET_METHOD(constructor_template, "findAll", NodeFtdi::FindAll);

    //exports->Set(String::NewSymbol("Ftdi"), constructor_template->GetFunction());
    module->Set(String::NewSymbol("exports"), constructor_template->GetFunction());
}

Handle<Value> NodeFtdi::Init(const Arguments& args) {
    HandleScope scope;

    int ret;

    if(initialized) {
        Close(args);
    }

    ret = ftdi_init(&ftdic);
    if (ret < 0) {
        return scope.Close(String::New("Failed to init"));
    }
    initialized = true;

    return scope.Close(Boolean::New(true));
}

Handle<Value> NodeFtdi::Open(const Arguments& args) {
    HandleScope scope;

    if(!initialized) {
        Init(args);
    }

    if(opened) {
        Close(args);
    }
    
    int vid = args[0]->IsUndefined() ? FTDI_VID : args[0]->Int32Value();
    int pid = args[1]->IsUndefined() ? FTDI_PID : args[1]->Int32Value();

    int ret;
    unsigned char bits[1];

    ret = ftdi_usb_open(&ftdic, vid, pid);
    if(ret < 0) {
        return scope.Close(String::New("Unable to open device"));
    }

    ret = ftdi_read_pins(&ftdic, bits);
    if(ret < 0) {        
        return scope.Close(String::New("Unable to read from device"));
    }

    //Board has just been plugged in or power lost
    if (bits[0] == 0xFF) {
        //Prevents 0xFF from flipping all switches on
        ret = ftdi_set_bitmode(&ftdic, 0xF0, BITMODE_BITBANG);    
        ftdi_write_data(&ftdic, bits, 1);
        ftdi_write_data(&ftdic, bits, 1);
    }
    ret = ftdi_set_bitmode(&ftdic, 0xFF, BITMODE_BITBANG);
    if(ret < 0) {
        return scope.Close(String::New("Unable to set bit mode"));
    }
    opened = true;

    return scope.Close(Boolean::New(true));
}


Handle<Value> NodeFtdi::GetPins(const Arguments& args) {
    HandleScope scope;

    unsigned char bits[1];

    ftdi_read_pins(&ftdic, bits);
    return scope.Close(Number::New((int) (0xF - (bits[0] - 0xF0))));
}



Handle<Value> NodeFtdi::SetPins(const Arguments& args) {
    HandleScope scope;

    unsigned char bits[1];
    int pins = args[0]->IsUndefined() ? 0 : args[0]->Int32Value();

    bits[0] = (char) (0xF0 + (0xF - pins));
    ftdi_write_data(&ftdic, bits, 1);
    ftdi_write_data(&ftdic, bits, 1);
    return scope.Close(Boolean::New(true));
}

Handle<Value> NodeFtdi::Close(const Arguments& args) {
    HandleScope scope;

    ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);

    initialized = false;
    opened = false;

    return scope.Close(Boolean::New(true));
}

Handle<Value> NodeFtdi::FindAll(const Arguments& args) {
    HandleScope scope;

    if(!initialized) {
        Init(args);
    }

    int count, i = 0;
    struct ftdi_device_list *devlist, *curdev;
    char manufacturer[128], description[128], serial[128];

    int vid = args[0]->IsUndefined() ? FTDI_VID : args[0]->Int32Value();
    int pid = args[1]->IsUndefined() ? FTDI_PID : args[1]->Int32Value();

    if ((count = ftdi_usb_find_all(&ftdic, &devlist, vid, pid)) < 0) {
        return scope.Close(String::New("Unable to list devices"));
    }

    Local<Array> array = Array::New(count);

    for (curdev = devlist; curdev != NULL; i++) {
        if (ftdi_usb_get_strings(&ftdic, curdev->dev, manufacturer, 128, description, 128, serial, 128) < 0) {
            return scope.Close(String::New("Unable to get information on devices"));
        }

        Local<Object> obj = Object::New();
        obj->Set(String::New("manufacturer"), String::New(manufacturer));
        obj->Set(String::New("description"), String::New(description));
        obj->Set(String::New("serial"), String::New(serial));
        array->Set(i, obj);
        curdev = curdev->next;
    }

    ftdi_list_free(&devlist);

    return scope.Close(array);
}

NODE_MODULE(ftdi, NodeFtdi::Initialize)