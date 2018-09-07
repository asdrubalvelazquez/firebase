/*
 * Copyright 2018 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.1 */

#include "document.nanopb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t google_firestore_v1beta1_Document_fields[5] = {
    PB_FIELD(  1, STRING  , SINGULAR, POINTER , FIRST, google_firestore_v1beta1_Document, name, name, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, POINTER , OTHER, google_firestore_v1beta1_Document, fields, name, &google_firestore_v1beta1_Document_FieldsEntry_fields),
    PB_FIELD(  3, MESSAGE , SINGULAR, STATIC  , OTHER, google_firestore_v1beta1_Document, create_time, fields, &google_protobuf_Timestamp_fields),
    PB_FIELD(  4, MESSAGE , SINGULAR, STATIC  , OTHER, google_firestore_v1beta1_Document, update_time, create_time, &google_protobuf_Timestamp_fields),
    PB_LAST_FIELD
};

const pb_field_t google_firestore_v1beta1_Document_FieldsEntry_fields[3] = {
    PB_FIELD(  1, STRING  , SINGULAR, POINTER , FIRST, google_firestore_v1beta1_Document_FieldsEntry, key, key, 0),
    PB_FIELD(  2, MESSAGE , SINGULAR, STATIC  , OTHER, google_firestore_v1beta1_Document_FieldsEntry, value, key, &google_firestore_v1beta1_Value_fields),
    PB_LAST_FIELD
};

const pb_field_t google_firestore_v1beta1_Value_fields[12] = {
    PB_ONEOF_FIELD(value_type,   1, BOOL    , ONEOF, STATIC  , FIRST, google_firestore_v1beta1_Value, boolean_value, boolean_value, 0),
    PB_ONEOF_FIELD(value_type,   2, INT64   , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, integer_value, integer_value, 0),
    PB_ONEOF_FIELD(value_type,   3, DOUBLE  , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, double_value, double_value, 0),
    PB_ONEOF_FIELD(value_type,   5, STRING  , ONEOF, POINTER , UNION, google_firestore_v1beta1_Value, reference_value, reference_value, 0),
    PB_ONEOF_FIELD(value_type,   6, MESSAGE , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, map_value, map_value, &google_firestore_v1beta1_MapValue_fields),
    PB_ONEOF_FIELD(value_type,   8, MESSAGE , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, geo_point_value, geo_point_value, &google_type_LatLng_fields),
    PB_ONEOF_FIELD(value_type,   9, MESSAGE , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, array_value, array_value, &google_firestore_v1beta1_ArrayValue_fields),
    PB_ONEOF_FIELD(value_type,  10, MESSAGE , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, timestamp_value, timestamp_value, &google_protobuf_Timestamp_fields),
    PB_ONEOF_FIELD(value_type,  11, ENUM    , ONEOF, STATIC  , UNION, google_firestore_v1beta1_Value, null_value, null_value, 0),
    PB_ONEOF_FIELD(value_type,  17, STRING  , ONEOF, POINTER , UNION, google_firestore_v1beta1_Value, string_value, string_value, 0),
    PB_ONEOF_FIELD(value_type,  18, BYTES   , ONEOF, POINTER , UNION, google_firestore_v1beta1_Value, bytes_value, bytes_value, 0),
    PB_LAST_FIELD
};

const pb_field_t google_firestore_v1beta1_ArrayValue_fields[2] = {
    PB_FIELD(  1, MESSAGE , REPEATED, POINTER , FIRST, google_firestore_v1beta1_ArrayValue, values, values, &google_firestore_v1beta1_Value_fields),
    PB_LAST_FIELD
};

const pb_field_t google_firestore_v1beta1_MapValue_fields[2] = {
    PB_FIELD(  1, MESSAGE , REPEATED, POINTER , FIRST, google_firestore_v1beta1_MapValue, fields, fields, &google_firestore_v1beta1_MapValue_FieldsEntry_fields),
    PB_LAST_FIELD
};

const pb_field_t google_firestore_v1beta1_MapValue_FieldsEntry_fields[3] = {
    PB_FIELD(  1, STRING  , SINGULAR, POINTER , FIRST, google_firestore_v1beta1_MapValue_FieldsEntry, key, key, 0),
    PB_FIELD(  2, MESSAGE , SINGULAR, STATIC  , OTHER, google_firestore_v1beta1_MapValue_FieldsEntry, value, key, &google_firestore_v1beta1_Value_fields),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(google_firestore_v1beta1_Document, create_time) < 65536 && pb_membersize(google_firestore_v1beta1_Document, update_time) < 65536 && pb_membersize(google_firestore_v1beta1_Document_FieldsEntry, value) < 65536 && pb_membersize(google_firestore_v1beta1_Value, value_type.map_value) < 65536 && pb_membersize(google_firestore_v1beta1_Value, value_type.geo_point_value) < 65536 && pb_membersize(google_firestore_v1beta1_Value, value_type.array_value) < 65536 && pb_membersize(google_firestore_v1beta1_Value, value_type.timestamp_value) < 65536 && pb_membersize(google_firestore_v1beta1_MapValue_FieldsEntry, value) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_google_firestore_v1beta1_Document_google_firestore_v1beta1_Document_FieldsEntry_google_firestore_v1beta1_Value_google_firestore_v1beta1_ArrayValue_google_firestore_v1beta1_MapValue_google_firestore_v1beta1_MapValue_FieldsEntry)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(google_firestore_v1beta1_Document, create_time) < 256 && pb_membersize(google_firestore_v1beta1_Document, update_time) < 256 && pb_membersize(google_firestore_v1beta1_Document_FieldsEntry, value) < 256 && pb_membersize(google_firestore_v1beta1_Value, value_type.map_value) < 256 && pb_membersize(google_firestore_v1beta1_Value, value_type.geo_point_value) < 256 && pb_membersize(google_firestore_v1beta1_Value, value_type.array_value) < 256 && pb_membersize(google_firestore_v1beta1_Value, value_type.timestamp_value) < 256 && pb_membersize(google_firestore_v1beta1_MapValue_FieldsEntry, value) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_google_firestore_v1beta1_Document_google_firestore_v1beta1_Document_FieldsEntry_google_firestore_v1beta1_Value_google_firestore_v1beta1_ArrayValue_google_firestore_v1beta1_MapValue_google_firestore_v1beta1_MapValue_FieldsEntry)
#endif


/* @@protoc_insertion_point(eof) */
