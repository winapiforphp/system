#include "..\php_winsystem.h"
#include <Winsvc.h>

// http://www.percyengine.com/images/sir-topham-hatt.jpg

static zend_function ctor_wrapper_func;
zend_class_entry *ce_winsystem_service_controller;
HashTable        winsystem_service_controller_prop_handlers;

/* ---------------------------------------------------------------
  Win\System\Service\Controller Userland API

  Static methods first, instance methods to follow.
-----------------------------------------------------------------*/

static void _populate_array_with_services_enumeration (zval *out_array, char *computer, int type TSRMLS_DC)
{
    ENUM_SERVICE_STATUS_PROCESS current_enum;
    SC_HANDLE scm_handle;
    LPBYTE    services = NULL;
    DWORD bytes_needed,      /* Total bytes needed to store all of the services information */
          services_returned; /* The amount of services returned */
    zval *service_array,
         *service_status_process_array;
    BOOL result;
    DWORD  i;

    scm_handle = OpenSCManager(computer, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE);
    if (scm_handle == NULL)
    {
        winsystem_create_error(GetLastError(), ce_winsystem_argexception TSRMLS_CC);
        return;
    }
    
    /* We need to first get information so we can allocate enough memory for the data */
    result = EnumServicesStatusEx(scm_handle, SC_ENUM_PROCESS_INFO, type,
        SERVICE_STATE_ALL, services, 0, &bytes_needed, &services_returned, NULL, NULL);
    if (result == FALSE && GetLastError() != ERROR_MORE_DATA)
    {
        winsystem_create_error(GetLastError(), ce_winsystem_argexception TSRMLS_CC);
        return;
    }

    services = malloc(bytes_needed * sizeof(BYTE));
    /* Now we can enumerate the services and retrieve the data */
    result = EnumServicesStatusEx(scm_handle, SC_ENUM_PROCESS_INFO, type,
        SERVICE_STATE_ALL, services, bytes_needed, &bytes_needed, &services_returned, NULL, NULL);
    if (result == FALSE)
    {
        winsystem_create_error(GetLastError(), ce_winsystem_argexception TSRMLS_CC);
        return;
    }

    array_init(out_array);
    /* TODO: signed/unsigned mismatch */
    for (i = 0; i < services_returned; i++)
    {
        current_enum = ((ENUM_SERVICE_STATUS_PROCESS *)services)[i];
        /* A new array */
        MAKE_STD_ZVAL(service_array);
        array_init(service_array);
        add_assoc_string(service_array, "ServiceName", current_enum.lpServiceName, 1);
        add_assoc_string(service_array, "DisplayName", current_enum.lpDisplayName, 1);
        /* And another array to hold the SERVICE_STATUS_PROCESS structure for the enum */
        MAKE_STD_ZVAL(service_status_process_array);
        array_init(service_status_process_array);
        add_assoc_long(service_status_process_array, "ServiceType", current_enum.ServiceStatusProcess.dwServiceType);
        add_assoc_long(service_status_process_array, "CurrentState", current_enum.ServiceStatusProcess.dwCurrentState);
        add_assoc_long(service_status_process_array, "ControlsAccepted", current_enum.ServiceStatusProcess.dwControlsAccepted);
        add_assoc_long(service_status_process_array, "Win32ExitCode", current_enum.ServiceStatusProcess.dwWin32ExitCode);
        add_assoc_long(service_status_process_array, "ServiceSpeficicExitCode", current_enum.ServiceStatusProcess.dwServiceSpecificExitCode);
        add_assoc_long(service_status_process_array, "CheckPoint", current_enum.ServiceStatusProcess.dwCheckPoint);
        add_assoc_long(service_status_process_array, "WaitHint", current_enum.ServiceStatusProcess.dwWaitHint);
        add_assoc_long(service_status_process_array, "ProcessID", current_enum.ServiceStatusProcess.dwProcessId);
        add_assoc_long(service_status_process_array, "ServiceFlags", current_enum.ServiceStatusProcess.dwServiceFlags);
        add_assoc_zval(service_array, "ServiceStatusProcess", service_status_process_array);
        add_next_index_zval(out_array, service_array);
    }
}


ZEND_BEGIN_ARG_INFO_EX(WinSystemServiceController_getServices_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, computer_name)
ZEND_END_ARG_INFO()

/**
 * Populates a zval array with data from the services enumeration
 */
PHP_METHOD(WinSystemServiceController, getServices)
{
    char *computer_name = NULL;
    int  computer_name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &computer_name, &computer_name_len) == FAILURE)
        return;

    _populate_array_with_services_enumeration(return_value, computer_name, SERVICE_WIN32 TSRMLS_CC);
}

ZEND_BEGIN_ARG_INFO_EX(WinSystemServiceController_getDevices_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, computer_name)
ZEND_END_ARG_INFO()

PHP_METHOD(WinSystemServiceController, getDevices)
{
    char *computer_name = NULL;
    int  computer_name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &computer_name, &computer_name_len) == FAILURE)
        return;

    _populate_array_with_services_enumeration(return_value, computer_name, SERVICE_DRIVER TSRMLS_CC);
}

ZEND_BEGIN_ARG_INFO_EX(WinSystemServiceController___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, service_name)
    ZEND_ARG_INFO(0, computer_name)
ZEND_END_ARG_INFO()

PHP_METHOD(WinSystemServiceController, __construct)
{

}

static zend_function_entry functions[] = {
    PHP_ME(WinSystemServiceController, getServices, WinSystemServiceController_getServices_args, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
    PHP_ME(WinSystemServiceController, getDevices,  WinSystemServiceController_getDevices_args,  ZEND_ACC_STATIC | ZEND_ACC_PUBLIC) 
    {NULL, NULL, NULL}
};

/* ---------------------------------------------------------------
  Win\System\Service\Controller Object Creation and Interaction
-----------------------------------------------------------------*/

int winsystem_service_controller_read_property(winsystem_generic_object *object, zval *member, zval **retval TSRMLS_DC)
{
    printf("Trying to read '%s' property\n", Z_STRVAL_P(member));
    ZVAL_STRING(*retval, Z_STRVAL_P(member), 1);

    return SUCCESS;
}

static void winsystem_service_controller_object_destroy(void *object TSRMLS_DC)
{
}

static zend_object_value winsystem_service_controller_object_create(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    winsystem_service_controller_object *object;
    zval *tmp;

    object = ecalloc(1, sizeof(winsystem_service_controller_object));
    object->std.ce = ce;
    object->std.guards = NULL;

    ALLOC_HASHTABLE(object->std.properties);
    zend_hash_init(object->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(object->std.properties, &ce->default_properties,
        (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(object, (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) winsystem_service_controller_object_destroy, NULL TSRMLS_CC);
    retval.handlers = &winsystem_object_handlers;
    object->zobject_handle = retval.handle;
    return retval;
}

/* ---------------------------------------------------------------
  Win\System\Service\Controller Lifecycle Functions
-----------------------------------------------------------------*/

PHP_MINIT_FUNCTION(winsystem_service_controller)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_SERVICE_NS, "Controller", functions);
    ce_winsystem_service_controller = zend_register_internal_class(&ce TSRMLS_CC);
    ce_winsystem_service_controller->create_object = winsystem_service_controller_object_create;


    zend_hash_init(&winsystem_service_controller_prop_handlers, 0, NULL, NULL, 1);
    #define SERVICE_REGISTER_PROP(name, read, write) \
        winsystem_register_prop_handler(&winsystem_service_controller_prop_handlers, ce_winsystem_service_controller, name, read, write TSRMLS_CC)
    SERVICE_REGISTER_PROP("canPauseAndContinue", winsystem_service_controller_read_property, NULL);

    return SUCCESS;
}