/******************************************************************************
 *                                                                            *
 * Copyright (C) 2024 MachineWare GmbH                                        *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 *                                                                            *
 ******************************************************************************/

#include "vcml/protocols/tlm.h"
#include "vcml/protocols/gpio.h"
#include "vcml/protocols/clk.h"
#include "vcml/protocols/sd.h"
#include "vcml/protocols/spi.h"
#include "vcml/protocols/i2c.h"
#include "vcml/protocols/pci.h"
#include "vcml/protocols/eth.h"
#include "vcml/protocols/can.h"
#include "vcml/protocols/usb.h"
#include "vcml/protocols/serial.h"
#include "vcml/protocols/virtio.h"

#include "vcml/tracing/tracer_inscight.h"

namespace vcml {

#ifdef HAVE_INSCIGHT

static inscight::protocol_kind inscight_protocol(protocol_kind kind) {
    switch (kind) {
    case PROTO_TLM:
        return inscight::PROTO_TLM;
    case PROTO_GPIO:
        return inscight::PROTO_GPIO;
    case PROTO_CLK:
        return inscight::PROTO_CLK;
    case PROTO_PCI:
        return inscight::PROTO_PCI;
    case PROTO_I2C:
        return inscight::PROTO_I2C;
    case PROTO_SPI:
        return inscight::PROTO_SPI;
    case PROTO_SD:
        return inscight::PROTO_SD;
    case PROTO_SERIAL:
        return inscight::PROTO_SERIAL;
    case PROTO_VIRTIO:
        return inscight::PROTO_VIRTIO;
    case PROTO_ETHERNET:
        return inscight::PROTO_ETHERNET;
    case PROTO_CAN:
        return inscight::PROTO_CAN;
    case PROTO_USB:
        return inscight::PROTO_USB;
    default:
        return inscight::PROTO_UNKNOWN;
    }
}

template <typename PAYLOAD>
string serialize(const PAYLOAD& tx) {
    return "{}"; // default empty
}

template <>
string serialize(const tlm_generic_payload& tx) {
    std::ostringstream os;
    os << std::hex << std::showbase;

    os << "{";
    os << "\"address\":\"" << tx.get_address() << "\",";
    os << "\"data\":[";
    unsigned char* data_ptr = tx.get_data_ptr();
    for (unsigned int i = 0; i < tx.get_data_length(); ++i) {
        os << "\"" << (u32)data_ptr[i] << "\"";
        if (i < tx.get_data_length() - 1)
            os << ",";
    }
    os << "],";

    os << "\"command\":";
    switch (tx.get_command()) {
    case TLM_READ_COMMAND:
        os << "\"READ\",";
        break;
    case TLM_WRITE_COMMAND:
        os << "\"WRITE\",";
        break;
    case TLM_IGNORE_COMMAND:
        os << "\"IGNORE\",";
        break;
    default:
        os << "\"UNKNOWN\",";
        break;
    }

    os << "\"byte_enable\":[";
    unsigned char* byte_enable_ptr = tx.get_byte_enable_ptr();
    for (unsigned int i = 0; i < tx.get_byte_enable_length(); ++i) {
        os << "\"" << (u32)byte_enable_ptr[i] << "\"";
        if (i < tx.get_byte_enable_length() - 1)
            os << ",";
    }
    os << "],";
    os << "\"streaming_width\":\"" << tx.get_streaming_width() << "\",";
    os << "\"dmi_allowed\":" << (tx.is_dmi_allowed() ? "true" : "false")
       << ",";
    os << "\"response_status\":\"" << tx.get_response_string() << "\"";

    if (tx_has_sbi(tx)) {
        os << ",\"sbi\":{";
        os << "\"is_debug\":" << (tx_is_debug(tx) ? "true" : "false") << ",";
        os << "\"is_nodmi\":" << (tx_is_nodmi(tx) ? "true" : "false") << ",";
        os << "\"is_sync\":" << (tx_is_sync(tx) ? "true" : "false") << ",";
        os << "\"is_insn\":" << (tx_is_insn(tx) ? "true" : "false") << ",";
        os << "\"is_excl\":" << (tx_is_excl(tx) ? "true" : "false") << ",";
        os << "\"is_lock\":" << (tx_is_lock(tx) ? "true" : "false") << ",";
        os << "\"is_secure\":" << (tx_is_secure(tx) ? "true" : "false") << ",";

        os << std::dec;
        os << "\"cpuid\":" << tx_cpuid(tx) << ",";
        os << "\"privilege\":" << tx_privilege(tx) << ",";
        os << "\"asid\":" << tx_asid(tx);
        os << "}";
    }

    os << "}";
    return os.str();
}

template <typename PAYLOAD>
void tracer_inscight::do_trace(const activity<PAYLOAD>& msg) {
    auto json = serialize(msg.payload);
    auto kind = inscight_protocol(msg.kind);
    auto time = time_to_ps(msg.t);
    if (is_backward_trace(msg.dir))
        INSCIGHT_TRANSACTION_TRACE_BW(msg.port, time, kind, json.c_str());
    else
        INSCIGHT_TRANSACTION_TRACE_FW(msg.port, time, kind, json.c_str());
}

#else

template <typename PAYLOAD>
void tracer_inscight::do_trace(const activity<PAYLOAD>& msg) {
    // nothing to do
}

#endif

void tracer_inscight::trace(const activity<tlm_generic_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<gpio_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<clk_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<pci_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<i2c_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<spi_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<sd_command>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<sd_data>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<vq_message>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<serial_payload>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<eth_frame>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<can_frame>& msg) {
    do_trace(msg);
}

void tracer_inscight::trace(const activity<usb_packet>& msg) {
    do_trace(msg);
}

tracer_inscight::tracer_inscight(): tracer() {
#ifndef HAVE_INSCIGHT
    log_warn("InSCight tracing not available");
#endif
}

tracer_inscight::~tracer_inscight() {
    // nothing to do
}

} // namespace vcml
