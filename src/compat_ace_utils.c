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

void dump_uuid2(const uuid_t* uuid) {
    log_debug(
        "[%s()]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        __func__, uuid->uu[0], uuid->uu[1], uuid->uu[2], uuid->uu[3], uuid->uu[4], uuid->uu[5],
        uuid->uu[6], uuid->uu[7], uuid->uu[8], uuid->uu[9], uuid->uu[10], uuid->uu[11],
        uuid->uu[12], uuid->uu[13], uuid->uu[14], uuid->uu[15]
    );
}

void dump_bleGattBlobValue_t(const bleGattBlobValue_t* value) {
    log_debug("Dumping bleGattBlobValue_t");
    log_debug(
        "bleGattBlobValue {\n"
        "  size %d\n"
        "  offset %d\n"
        "  data NEXT\n"
        "}",
        value->size, value->offset
    );
    printf("bleGattBlobValue.data = ");
    for (uint16_t i = 0; i < value->size; ++i) {
        printf("0x%02X ", value->data[i]);
    }
    printf("\n");
}

void dump_bleGattRecord_t(const bleGattRecord_t* record) {
    log_debug("Dumping bleGattRecord_t");
    dump_uuid2(&record->uuid);
    log_debug(
        "bleGattRecord_t {\n"
        "  uuid PREV\n"
        "  attProp %d\n"
        "  attPerm %d\n"
        "  handle %d (%p)\n"
        "}",
        record->attProp, record->attPerm, record->handle, record->handle
    );
}

void dump_bleGattDescriptor_t(const bleGattDescriptor_t* descriptor) {
    if (descriptor == NULL) {
        log_debug("Dumping bleGattDescriptor_t: NULL");
        return;
    }

    log_debug("Dumping bleGattDescriptor_t");
    log_debug(
        "bleGattDescriptor_t {\n"
        "  gattRecord NEXT\n"
        "  blobValue NEXT\n"
        "  is_set %d\n"
        "  is_notify %d\n"
        "  desc_auth_retry %d\n"
        "  write_type %d\n"
        "}",
        descriptor->is_set, descriptor->is_notify, descriptor->desc_auth_retry,
        descriptor->write_type
    );

    dump_bleGattRecord_t(&descriptor->gattRecord);
    dump_bleGattBlobValue_t(&descriptor->blobValue);
}

void dump_bleGattCharacteristicsValue_t(const bleGattCharacteristicsValue_t chars_value) {
    log_debug("Dumping bleGattCharacteristicsValue_t");
    log_debug(
        "bleGattCharacteristicsValue_t {\n"
        "  format %d\n"
        "  gattRecord NEXT\n"
        "  gattDescriptor NEXT\n"
        "  auth_retry %d\n"
        "  read_auth_retry %d\n"
        "  write_type %d\n"
        "  descList NEXT\n"
        "  multiDescCount %d\n"
        "}",
        chars_value.format, chars_value.auth_retry, chars_value.read_auth_retry,
        chars_value.write_type, chars_value.multiDescCount
    );

    if (chars_value.format == 255) {
        dump_bleGattBlobValue_t(&chars_value.blobValue);
    }

    dump_bleGattRecord_t(&chars_value.gattRecord);
    log_debug("Main descriptor:");
    dump_bleGattDescriptor_t(&chars_value.gattDescriptor);

    log_debug("Linked list descriptors:");
    if (chars_value.multiDescCount) {
        struct aceBT_gattDescRec_t* desc_rec = NULL;
        STAILQ_FOREACH(desc_rec, &chars_value.descList, link) {
            dump_bleGattDescriptor_t(&desc_rec->value);
        }
    }
    log_debug("Done linked list descriptors");
}

void dump_notify_data_t(const notify_data_t* data) {
    log_debug("Dumping notify_data_t");
    log_debug(
        "notify_data_t {\n"
        "  size %d\n"
        "  conn_handle %d (%p)\n"
        "  session_handle %d (%p)\n"
        // "  confirm %d\n"
        "  value NEXT\n"
        "  data_len %d\n"
        "  data %p SKIPPED\n"
        "}",
        data->size, data->conn_handle, data->conn_handle, data->session_handle,
        data->session_handle,
        // data->confirm,
        data->data_len, data->data
    );

    dump_bleGattCharacteristicsValue_t(data->value);
}
