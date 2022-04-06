/******************************************************************************
 *                                                                            *
 * Copyright 2018 Jan Henrik Weinstock                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 *                                                                            *
 ******************************************************************************/

#include "vcml/protocols/sd.h"

namespace vcml {

const char* sd_status_str(sd_status status) {
    switch (status) {
    case SD_INCOMPLETE:
        return "SD_INCOMPLETE";
    case SD_OK:
        return "SD_OK";
    case SD_OK_TX_RDY:
        return "SD_OK_TX_RDY";
    case SD_OK_RX_RDY:
        return "SD_OK_RX_RDY";
    case SD_ERR_CRC:
        return "SD_ERR_CRC";
    case SD_ERR_ARG:
        return "SD_ERR_ARG";
    case SD_ERR_ILLEGAL:
        return "SD_ERR_ILLEGAL";
    default:
        return "unknown";
    }
}

const char* sd_status_str(sd_status_tx status) {
    switch (status) {
    case SDTX_INCOMPLETE:
        return "SDTX_INCOMPLETE";
    case SDTX_OK:
        return "SDTX_OK";
    case SDTX_OK_BLK_DONE:
        return "SDTX_OK_BLK_DONE";
    case SDTX_OK_COMPLETE:
        return "SDTX_OK_COMPLETE";
    case SDTX_ERR_ILLEGAL:
        return "SDTX_ERR_ILLEGAL";
    default:
        return "unknown";
    }
}

const char* sd_status_str(sd_status_rx status) {
    switch (status) {
    case SDRX_INCOMPLETE:
        return "SDRX_INCOMPLETE";
    case SDRX_OK:
        return "SDRX_OK";
    case SDRX_OK_BLK_DONE:
        return "SDRX_OK_BLK_DONE";
    case SDRX_OK_COMPLETE:
        return "SDRX_OK_COMPLETE";
    case SDRX_ERR_CRC:
        return "SDRX_ERR_CRC";
    case SDRX_ERR_INT:
        return "SDRX_ERR_INT";
    case SDRX_ERR_ILLEGAL:
        return "SDRX_ERR_ILLEGAL";
    default:
        return "unknown";
    }
}

static const char* do_cmd_str(u8 opcode) {
    switch (opcode) {
    case 0:
        return "GO_IDLE_STATE";
    case 1:
        return "SEND_OP_COND";
    case 2:
        return "ALL_SEND_CID";
    case 3:
        return "SEND_RELATIVE_ADDR";
    case 4:
        return "SET_DSR";
    case 5:
        return "SD_IO_SEND_OP_COND";
    case 6:
        return "SWITCH_FUNC";
    case 7:
        return "SELECT_DESELECT_CARD";
    case 8:
        return "SEND_IF_COND";
    case 9:
        return "SEND_CSD";
    case 10:
        return "SEND_CID";
    case 11:
        return "VOLTAGE_SWITCH";
    case 12:
        return "STOP_TRANSMISSION";
    case 13:
        return "SEND_STATUS";
    case 15:
        return "GO_INACTIVE_STATE";
    case 16:
        return "SET_BLOCKLEN";
    case 17:
        return "READ_SINGLE_BLOCK";
    case 18:
        return "READ_MULTIPLE_BLOCK";
    case 19:
        return "SEND_TUNING_BLOCK";
    case 20:
        return "SPEED_CLASS_CONTROL";
    case 23:
        return "SET_BLOCK_COUNT";
    case 24:
        return "WRITE_BLOCK";
    case 25:
        return "WRITE_MULTIPLE_BLOCK";
    case 26:
        return "RESERVED_26";
    case 27:
        return "PROGRAM_CSD";
    case 28:
        return "SET_WRITE_PROT";
    case 29:
        return "CLR_WRITE_PROT";
    case 30:
        return "SEND_WRITE_PROT";
    case 32:
        return "ERASE_WR_BLK_START_ADDR";
    case 33:
        return "ERASE_WR_BLK_END_ADDR";
    case 38:
        return "ERASE";
    case 42:
        return "LOCK_UNLOCK";
    case 48:
        return "READ_EXTR_SINGLE";
    case 49:
        return "WRITE_EXTR_SINGLE";
    case 52:
        return "IO_RW_DIRECT";
    case 53:
        return "IO_RW_EXTENDED";
    case 55:
        return "APP_CMD";
    case 56:
        return "GEN_CMD";
    case 58:
        return "READ_OCR";
    case 59:
        return "CRC_ON_OFF";
    case 60:
        return "RESERVED_60";
    case 61:
        return "RESERVED_61";
    case 62:
        return "RESERVED_62";
    case 63:
        return "RESERVED_63";
    default:
        return "UNKNOWN";
    }
}

static const char* do_acmd_str(u8 opcode) {
    switch (opcode) {
    case 6:
        return "SET_BUS_WIDTH";
    case 13:
        return "SD_STATUS";
    case 22:
        return "SEND_NUM_WR_BLOCKS";
    case 23:
        return "SET_WR_BLK_ERASE_COUNT";
    case 41:
        return "SD_SEND_OP_COND";
    case 42:
        return "SET_CLR_CARD_DETECT";
    case 51:
        return "SEND_SCR";
    default:
        return "UNKNOWN";
    }
}

const char* sd_opcode_str(u8 opcode, bool appcmd) {
    return appcmd ? do_acmd_str(opcode) : do_cmd_str(opcode);
}

#define HEXW(w) std::hex << std::setw(w) << std::setfill('0')

ostream& operator<<(ostream& os, const sd_command& tx) {
    stream_guard guard(os);

    os << (tx.appcmd ? "SD-ACMD" : "SD-CMD") << std::dec << (int)tx.opcode;
    os << " " << sd_opcode_str(tx.opcode) << " (";
    os << HEXW(8) << tx.argument << ")";

    if (tx.resp_len > 0) {
        os << " [";
        for (unsigned int i = 0; i < tx.resp_len - 1; i++)
            os << HEXW(2) << (int)tx.response[i] << " ";
        os << HEXW(2) << (int)tx.response[tx.resp_len - 1] << "]";
    }

    os << " (" << sd_status_str(tx.status) << ")";
    return os;
}

ostream& operator<<(ostream& os, const sd_data& tx) {
    stream_guard guard(os);

    switch (tx.mode) {
    case SD_READ:
        os << "SD-DATA read";
        if (success(tx))
            os << mkstr(" [%02hhx]", tx.data);
        os << " (" << sd_status_str(tx.status.read) << ")";
        break;

    case SD_WRITE:
        os << mkstr("SD-DATA write [%02hhx]", tx.data);
        os << " (" << sd_status_str(tx.status.write) << ")";
        break;

    default:
        os << "DATA unknown";
    }

    return os;
}

sd_host::sd_target_sockets sd_host::all_sd_target_sockets(address_space as) {
    sd_target_sockets sockets;
    for (auto& socket : m_target_sockets)
        if (socket->as == as)
            sockets.push_back(socket);
    return sockets;
}

sd_base_initiator_socket::sd_base_initiator_socket(const char* nm,
                                                   address_space a):
    sd_base_initiator_socket_b(nm, a), m_stub(nullptr) {
}

sd_base_initiator_socket::~sd_base_initiator_socket() {
    if (m_stub)
        delete m_stub;
}

void sd_base_initiator_socket::stub() {
    VCML_ERROR_ON(m_stub, "socket '%s' already stubbed", name());
    hierarchy_guard guard(this);
    m_stub = new sd_target_stub(basename());
    bind(m_stub->sd_in);
}

sd_base_target_socket::sd_base_target_socket(const char* nm, address_space a):
    sd_base_target_socket_b(nm, a), m_stub(nullptr) {
}

sd_base_target_socket::~sd_base_target_socket() {
    if (m_stub)
        delete m_stub;
}

void sd_base_target_socket::stub() {
    VCML_ERROR_ON(m_stub, "socket '%s' already stubbed", name());
    hierarchy_guard guard(this);
    m_stub = new sd_initiator_stub(basename());
    m_stub->sd_out.bind(*this);
}

sd_initiator_socket::sd_initiator_socket(const char* nm, address_space as):
    sd_base_initiator_socket(nm, as),
    m_host(dynamic_cast<sd_host*>(hierarchy_top())),
    m_transport(this) {
    bind(m_transport);
    if (m_host)
        m_host->m_initiator_sockets.push_back(this);
}

sd_initiator_socket::~sd_initiator_socket() {
    if (m_host)
        stl_remove_erase(m_host->m_initiator_sockets, this);
}

void sd_initiator_socket::transport(sd_command& cmd) {
    trace_fw(cmd);
    (*this)->sd_transport(cmd);
    trace_bw(cmd);
}

void sd_initiator_socket::transport(sd_data& data) {
    // trace_fw(data);
    (*this)->sd_transport(data);
    // trace_bw(data);
}

sd_status_tx sd_initiator_socket::read_data(u8& data) {
    sd_data tx;
    tx.mode        = SD_READ;
    tx.data        = data;
    tx.status.read = SDTX_INCOMPLETE;

    transport(tx);
    data = tx.data;
    return tx.status.read;
}

sd_status_rx sd_initiator_socket::write_data(u8 data) {
    sd_data tx;
    tx.mode         = SD_WRITE;
    tx.data         = data;
    tx.status.write = SDRX_INCOMPLETE;

    transport(tx);
    return tx.status.write;
}

void sd_target_socket::sd_transport(sd_command& tx) {
    trace_fw(tx);
    m_host->sd_transport(*this, tx);
    trace_bw(tx);
}

void sd_target_socket::sd_transport(sd_data& tx) {
    // trace_fw(tx);
    m_host->sd_transport(*this, tx);
    // trace_bw(tx);
}

sd_target_socket::sd_target_socket(const char* nm, address_space a):
    sd_base_target_socket(nm, a),
    m_host(hierarchy_search<sd_host>()),
    m_transport(this) {
    bind(m_transport);
    VCML_ERROR_ON(!m_host, "%s declared outside sd_host", name());
    m_host->m_target_sockets.push_back(this);
}

sd_target_socket::~sd_target_socket() {
    if (m_host)
        stl_remove_erase(m_host->m_target_sockets, this);
}

sd_initiator_stub::sd_initiator_stub(const char* nm):
    sd_bw_transport_if(), sd_out(mkstr("%s_stub", nm).c_str()) {
    sd_out.bind(*(sd_bw_transport_if*)this);
}

void sd_target_stub::sd_transport(sd_command& cmd) {
    cmd.resp_len = 0;
    cmd.status   = SD_OK;
}

void sd_target_stub::sd_transport(sd_data& data) {
    if (data.mode == SD_READ)
        data.status.read = SDTX_ERR_ILLEGAL;
    else
        data.status.write = SDRX_OK;
}

sd_target_stub::sd_target_stub(const char* nm):
    sd_fw_transport_if(), sd_in(mkstr("%s_stub", nm).c_str()) {
    sd_in.bind(*this);
}

} // namespace vcml
