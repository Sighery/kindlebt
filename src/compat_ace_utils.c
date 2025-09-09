#include "compat_ace_utils.h"

#include <stddef.h>
#include <stdio.h>

#include "log.h"

#include "compat_ace_internals.h"

void dump_hex(const void* ptr, size_t size) {
    const unsigned char* data = (const unsigned char*)ptr;

    // Each byte is "XX ", 3 chars. One '\n' every 16 bytes, and a final '\0'
    size_t lines = (size + 15) / 16;
    size_t buf_len = (size * 3) + lines + 1;
    char* buf = malloc(buf_len);
    if (!buf) return;

    char* p = buf;
    for (size_t i = 0; i < size; ++i) {
        p += snprintf(p, buf + buf_len - p, "%02x ", data[i]);

        // Newline every 16 bytes
        if ((i + 1) % 16 == 0) *p++ = '\n';
    }
    *p = '\0';
    log_debug("Dumping hex:\n%s", buf);
    free(buf);
}

void dump_registerCbackGattcData(const registerCbackGattcData_t* data) {
    if (data == NULL) {
        log_warn("Dumping registerCbackGattcData_t: NULL");
        return;
    }

    log_debug(
        "Dumping registerCbackGattcData_t {\n"
        "  size: %u\n"
        "  session_handle: %p\n"
        "  callback_mask: %u\n"
        "  is_unregister: %u\n"
        "  app_id: %d\n"
        "  out_status: %d\n"
        "}",
        data->size, data->session_handle, data->callback_mask, data->is_unregister, data->app_id,
        data->out_status
    );
}

void dump_gattc_get_db_data_t(const gattc_get_db_data_t* data) {
    if (data == NULL) {
        log_warn("Dumping gattc_get_db_data_t: NULL");
        return;
    }

    log_debug(
        "Dumping gattc_get_db_data_t {\n"
        "  size: %d\n"
        "  conn_handle: %p\n"
        "  uuid: %p\n"
        "}",
        data->size, data->conn_handle, data->uuid
    );
}

void dump_aceAipc_parameter_t(aceAipc_parameter_t* task) {
    if (task == NULL) {
        log_warn("Dumping aceAipc_parameter_t: NULL");
        return;
    }

    log_debug(
        "Dumping aceAipc_parameter_t {\n"
        "  reserved0 %u (%p)\n"
        "  size %u (%p)\n"
        "  msg_type %u (%p)\n"
        "  function_id %u (%p)\n"
        "  cb1 %p\n"
        "  cb2 %p\n"
        "  buffer %p\n"
        "  reserved1 %u (%p)\n"
        "  reserved2 %u (%p)\n"
        "  reserved3 %u (%p)\n"
        "  flags %u (%p)\n"
        "  reserved4 %u (%p)\n"
        "  callback_id %u (%p)\n"
        "  server_id %u (%p)\n"
        "}",
        task->reserved0, task->reserved0, task->size, task->size, task->msg_type, task->msg_type,
        task->function_id, task->function_id, task->cb1, task->cb2, task->buffer, task->reserved1,
        task->reserved1, task->reserved2, task->reserved2, task->reserved3, task->reserved3,
        task->flags, task->flags, task->reserved4, task->reserved4, task->callback_id,
        task->callback_id, task->server_id, task->server_id
    );
}

void dump_mask_bits(uint16_t mask) {
    const int len = 64;
    char* buf = malloc(len);
    if (!buf) return;

    char* p = buf;
    for (int i = 15; i >= 0; --i) {
        *p++ = (mask & (1u << i)) ? '1' : '0';
        // Add spacing every 4 bits
        if (i % 4 == 0 && i != 0) *p++ = ' ';
    }
    *p = '\0';

    log_debug("Dumping mask: 0x%04x (binary: %s)", mask, buf);
    free(buf);
}

void dump_aipc_handle(aipcHandles_t aipc_handle) {
    log_debug(
        "aipc_handle {\n  callback_server_id: %u (0x%04x),\n  server_id: %u (0x%04x)\n}",
        aipc_handle.callback_server_id, aipc_handle.callback_server_id, aipc_handle.server_id,
        aipc_handle.server_id
    );
}
