#include "vcml/debugging/snapshotter.h"

namespace vcml {
namespace debugging {

snapshotter* snapshotter::instance = nullptr;


snapshotter::snapshotter(const string& name):suspender(name){
    instance = this;
}

snapshotter::snapshotter(const string& name, bool load_on_startup_set):suspender(name){
    instance = this;
    load_on_startup = load_on_startup_set;
}

snapshotter::~snapshotter(){
    instance = nullptr;
}

snapshotter* snapshotter::get_instance(){
    return instance;
}

void snapshotter::register_snapshottable(snapshotable* snapshottable){
    //TODO synchronization ?
    snapshotables.push_back(snapshottable);
}

void snapshotter::take_system_snapshot(){
    //TODO synchronization ?
    log_info("Taking system snapshot!");

    bool simulation_supended = simulation_suspended();
    if(!simulation_supended) suspend();

    for (size_t i = 0; i < snapshotables.size(); i++) {
        snapshotables[i]->take_snapshot();
    }

    log_info("Finished taking system snapshot!");

    if(!simulation_supended) resume();
}

void snapshotter::load_system_snapshot(){
    //TODO synchronization ?
    log_info("Loading system snapshot!");

    bool simulation_supended = simulation_suspended();
    if(!simulation_supended) suspend();

    for (size_t i = 0; i < snapshotables.size(); i++) {
        snapshotables[i]->load_snapshot();
    }

    log_info("Finished loading system snapshot!");

    if(!simulation_supended) resume();
}

void snapshotter::set_load_on_startup(bool set){
    load_on_startup = set;
}

bool snapshotter::is_load_on_startup(){
    return load_on_startup;
}


} // namespace debugging
} // namespace vcml
