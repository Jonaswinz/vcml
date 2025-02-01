#ifndef VCML_SNAPSHOTABLE_H
#define VCML_SNAPSHOTABLE_H

#include <fstream>

namespace vcml {
namespace debugging {

class snapshotable{

    private:
        
        bool enabled = false;
        std::string filename;

    public:
        
        bool take_snapshot();
        bool load_snapshot();

        void enable_snapshotting(const std::string& name);

        void load_snapshot_on_startup();

    protected:

        virtual bool on_snapshot_record(std::ofstream &ofs);
        virtual bool on_snapshot_restore(std::ifstream &ifs);
};


} // namespace debugging
} // namespace vcml

#endif
