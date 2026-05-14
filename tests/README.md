# Tests

## Run

From repo root:

```bash
make test
```

From the `tests/` folder:

```bash
make -C tests test
```

## File Guide

- `TestHarness.h`: lightweight shared assertions and test registration.
- `TestMain.cpp`: shared test runner (`main`) used by test binaries.
- `CoreLogicTests.cpp`: Synthem model/core logic tests.
- `DirectUITests.cpp`: DirectUI utility/core tests.
- `HeadlessControlTests.cpp`: headless control construction/mutation/hierarchy smoke tests.
- `StyleRegistryTests.cpp`: style registry registration/apply/normalization behavior tests.
- `stubs/`: test-only platform/rendering replacements for headless builds.
- `Makefile`: builds and runs the test binaries.

## Expected Logs

Some test logs are expected and not failures, including:

- missing default style logs (for headless control tests),
- duplicate style registration log (intentional duplicate registration behavior test),
- typed style mismatch log (intentional style-type mismatch test).

## Guardrail

Production source/header files should not be modified just to make tests easier unless explicitly approved.
