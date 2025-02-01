#ifndef VCML_SNAPSHOTTER_H
#define VCML_SNAPSHOTTER_H

#include "snapshotable.h"
#include "vcml/debugging/suspender.h"
#include "vcml/logging/logger.h"
#include <vector>

namespace vcml {
namespace debugging {

class snapshotter: public suspender{

    public:
        snapshotter(const string& name);

        snapshotter(const string& name, bool load_on_startup_set);

        ~snapshotter();

        static snapshotter* get_instance();

        void register_snapshottable(snapshotable* snapshottable);  

        void take_system_snapshot();

        void load_system_snapshot();

        void set_load_on_startup(bool set);

        bool is_load_on_startup();

    private:
        static snapshotter* instance;

        std::vector<snapshotable*> snapshotables;

        bool load_on_startup = false;
};


} // namespace debugging
} // namespace vcml

#endif
