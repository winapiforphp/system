#include "..\php_winsystem.h"
#include <Winsvc.h>

/* ----------------------------------------------------------------
  \Win\System\Service LifeCycle Functions

  This file handles the creation of classes and enumerations.
  You can find each class's implementation in the relevant file.
  Enumerations, however, will be implemented in this file.
------------------------------------------------------------------*/

PHP_MINIT_FUNCTION (winsystem_service)
{
    PHP_MINIT(winsystem_service_controller)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}