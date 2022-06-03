# core::log

Logging

The most important part of the engine, being able to log information and report errors

There are two main types of logs, logs when initializing core, and any logs after that.

## Core errors

The engine will create a file called ```core_status.txt``` to log the core status. If the file cannot be created,
the engine should quit without doing anything.

When core is being initialized, ```jn_log_core``` should be called before anything else with the ```JN_LOG_BEG``` flag.

After core has been initialized, call ```jn_log_core``` with the ```JN_LOG_END``` flag.

If an error occurs in any part of the core initialization, the ```jn_log_core``` function with the ```JN_LOG_LOG``` flag 
should be used to log the error.

## Other errors and logging

A logging guideline to use is for information that updates a lot (like position), use ```JN_QLOG``` (quick log),
and for information which updates less frequently use ```JN_LOG```, like opening a level file and outputting the size of the level.
```JN_QLOG``` outputs to ```stdout``` while ```JN_LOG``` outputs to a file (```log.txt```).

Use the macro to log, not the function. By not defining ```JN_DEV```, no logs will be made to slightly improve performance, and the
code will not need to be changed.

## Reference table

- [Types](#Types)
  - [enum jn_log](#enum-jn_log)
- [Functions](#Functions)
  - [jn_log_core](#jn_log_core)
  - [jn_log_init](#jn_log_init)
  - [jn_log_quit](#jn_log_quit)
  - [jn_log]()
- [Macros](#Macros)
  - [JN_LOG](#JN_LOG)
  - [JN_QLOG](#JN_QLOG)

## Reference

### Types

#### enum jn_log

Enumeration of log flags for ```jn_log_core```.

- JN_LOG_BEG (Initialize core logging)
- JN_LOG_END (Quit core logging)
- JN_LOG_LOG (Log a message during core initialization)

### Functions

#### jn_log_core

```int jn_log_core(enum jn_err flag, cont char *fmt, ...)```

Logger for the core module

##### params

###### flag

How the log should behave (init, quit, or log)

###### fmt

printf style format

###### ...

Variadic arguments to print

##### return

0 on success, -1 on failure

#### jn_log_init

```int jn_log_init(void)```

Initializes the logger by creating the ```log.txt``` text file.

##### return

0 on success, -1 on failure

#### jn_log_quit

```void jn_log_quit(void)```

Cleans up logger resources.

#### jn_log

```int jn_log(const char *fmt, ...)```

Logs to the ```log.txt``` file. Note if the logger fails, there really isn't a way to output the failure.
Since it is not too severe of an error, nothing will be done so if the log function fails so it will just be
undefined behavior.

##### params

###### fmt

printf style format

###### ...

Variadic arguments to print

##### return

0 on success, -1 on failure

### Macros

#### JN_LOG

Wrapper for ```jn_log```

If ```JN_DEV``` is not defined, this will not do anything

##### params

###### fmt

printf style format

###### ...

Variadic arguments to print

#### JN_QLOG

Logs to a console