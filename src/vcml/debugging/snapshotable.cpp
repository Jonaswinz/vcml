#include "vcml/debugging/snapshotable.h"
#include "vcml/debugging/snapshotter.h"

namespace vcml {
namespace debugging {

bool snapshotable::on_snapshot_record(std::ofstream &ofs){
    return false; // to be overwritten
}

bool snapshotable::on_snapshot_restore(std::ifstream &ifs){
    return false; // to be overwritten
}

void snapshotable::enable_snapshotting(const std::string& name){
    snapshotter::get_instance()->register_snapshottable(this);
    enabled = true;
    filename = "snapshot_"+name+".bin";
}

bool snapshotable::take_snapshot(){

    log_info("Taking snapshot %s.", filename.c_str());

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs){
        MWR_ERROR("Failed to open file %s for storing state!", filename.c_str());
        return false;
    } 

    bool success = on_snapshot_record(ofs);
    
    ofs.close();

    return success;
}

bool snapshotable::load_snapshot(){

    log_info("Loading snapshot %s.", filename.c_str());

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs){
        MWR_ERROR("Failed to open file %s for restoring state!", filename.c_str());
        return false;
    } 

    bool success = on_snapshot_restore(ifs);

    ifs.close();

    return success;
}

void snapshotable::load_snapshot_on_startup(){
    if(snapshotter::get_instance()->is_load_on_startup() && enabled) load_snapshot();
}


} // namespace debugging
} // namespace vcml
