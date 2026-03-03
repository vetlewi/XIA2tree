### User sorting plugins

This document explains how to extend XIA2tree with your own sorting plugins. A plugin is a shared library that receives each triggered event, can create its own histograms, and can apply arbitrary analysis logic.

---

### Conceptual overview

During the sorting process, after the built-in histograms are filled for an event, XIA2tree forwards the event to an optional **user sort plugin**:

- The plugin implements a `UserSort` interface.
- XIA2tree loads the plugin at runtime via `dlopen`/`dlsym` (or platform equivalent).
- For each `Triggered_event`, the plugin’s `FillEvent` method is called.
- At the end of the run, the plugin’s `Flush` method is called.

This allows you to:

- Add experiment-specific histograms.
- Perform custom gating or selections.
- Write additional results to the same ROOT file (via the shared histogram manager).

---

### Loading a plugin

To load a plugin at runtime, pass the shared library path with `-u` / `--userSort`:

```bash
XIA2tree \
  -i run.data \
  -o run.root \
  -C config.yml \
  -u libMyUserSort.so
```

The library is loaded once at startup. If loading or symbol resolution fails, XIA2tree will print an error and continue without plugin analysis.

---

### Plugin interface

Plugins use the `UserSort` interface declared in `include/UserSort/UserSort.h`. Conceptually, it looks like:

```cpp
class UserSort {
public:
    virtual ~UserSort() = default;

    // Called for every triggered event that passes the core analysis.
    virtual void FillEvent(const Triggered_event &event) = 0;

    // Called once at the end of the run to flush any buffered results.
    virtual void Flush() = 0;
};
```

A plugin must:

- Provide a concrete class derived from `UserSort`.
- Export a factory function (`extern "C"`) with a name XIA2tree expects (e.g. `NewUserSort`).
- Use the provided histogram manager and user configuration to create histograms and access parameters.

The exact function signature is visible in the existing plugins, but conceptually it is:

```cpp
extern "C" UserSort* NewUserSort(
    ThreadSafeHistograms* histManager,
    const OCL::UserConfiguration* config
);
```

`DynamicLibrary` in `src/UserSort/DynamicLibrary.cpp` is responsible for locating and calling this factory.

---

### Data structures available to plugins

In your plugin, you typically interact with:

- **`Triggered_event`**:
  - Represents one event built by the trigger stage.
  - Conceptually contains:
    - `std::vector<Entry_t>`: all hits in the event.
    - An index pointing to the trigger hit (or `-1` if no single trigger).
  - Provides helpers to:
    - Iterate over hits.
    - Query detectors by type and ID.
    - Access the trigger hit.

- **`Entry_t`**:
  - Represents a single calibrated hit.
  - Fields include:
    - Detector type (e.g. `labr`, `deDet`, `eDet`, `ppac`).
    - Detector ID (index).
    - Calibrated energy.
    - Corrected time / timestamp.
    - Other per-hit information (CFD, QDC, etc.).

- **`ThreadSafeHistograms`**:
  - A thread-safe container for ROOT histograms.
  - Provides methods to:
    - Create new 1D/2D histograms.
    - Fill them from multiple threads safely.

- **`OCL::UserConfiguration`**:
  - Gives access to:
    - Analysis parameters (gates, excitation coefficients, thickness windows).
    - The original YAML `user_parameters` subtree.
  - Allows plugins to read arbitrary configuration values specific to a given experiment.

You can inspect `src/Tasks/MTSort.cpp` and the existing plugins for concrete usage patterns.

---

### Example plugin skeleton

Below is a minimal, illustrative outline of a custom plugin. It omits some types and details for brevity, but shows the general structure.

```cpp
#include "UserSort/UserSort.h"
#include "Histogram/ThreadSafeHistograms.hpp" // or similar, depending on include paths
#include "PhysicalParam/ConfigManager.h"      // for OCL::UserConfiguration

class MyUserSort : public UserSort {
public:
    MyUserSort(ThreadSafeHistograms* h, const OCL::UserConfiguration* cfg)
        : hist(h), config(cfg)
    {
        // Create your histograms here using hist->Make1D / Make2D / etc.
        // Example (pseudo-code):
        // hMyEnergy = hist->Make1D("my_energy", "My detector energy", 2000, 0, 20000);
    }

    void FillEvent(const Triggered_event& event) override {
        // Inspect the event, select detectors, fill histograms.
        // Example (pseudo-code):
        //
        // for (auto& hit : event.Hits()) {
        //     if (hit.detectorType == DetectorType::labr) {
        //         hMyEnergy->Fill(hit.energy_cal);
        //     }
        // }
    }

    void Flush() override {
        // Optional: finalize or normalize histograms.
    }

private:
    ThreadSafeHistograms* hist;
    const OCL::UserConfiguration* config;

    // Pointers/handles to your histograms go here.
};

extern "C" UserSort* NewUserSort(
    ThreadSafeHistograms* histManager,
    const OCL::UserConfiguration* config
) {
    return new MyUserSort(histManager, config);
}
```

Adapt the includes and types to match your local include paths and the exact histogram interface used in this project.

---

### Building a plugin

You can either:

- Add a new `MODULE` library target to this repository’s `CMakeLists.txt`, or
- Build your plugin in a separate project that links against the installed `XIA2tree` and `OCL::Histogram` libraries.

As an example, the repository already defines plugins like `ParticleCoincidenceSort`, `Co60CoincidenceSort` and `TimingInvestigation` in the top-level `CMakeLists.txt`. They are built roughly as:

```cmake
add_library(ParticleCoincidenceSort MODULE ParticleCoincidenceSort.cpp)
target_link_libraries(ParticleCoincidenceSort
    PRIVATE
        OCL::UserSort
        XIAfuncs
        OCL::Histogram
        zstr
)
```

Key points:

- Use `add_library(... MODULE ...)` so that CMake builds a plugin (`.so` on Linux, `.dylib` on macOS).
- Link against:
  - `OCL::UserSort` (for the `UserSort` interface).
  - `XIAfuncs` (core data structures and helpers).
  - `OCL::Histogram` (for thread-safe histograms and ROOT integration).
- Optionally link additional dependencies you need (e.g. `zstr` for compressed input).

After building, you will typically get a file like:

```text
libMyUserSort.so    # or .dylib on macOS
```

Point XIA2tree at this file via `-u`.

---

### Example: Timing investigation plugin

The repository includes `TimingInvestigation.cpp` as an example plugin:

- It is compiled as a `MODULE` library.
- It links against `OCL::UserSort`, `XIAfuncs`, `OCL::Histogram`, and `zstr`.
- It implements detailed timing histograms and/or diagnostics tailored for a specific setup.

You can:

- Use it as-is by passing `-u libTimingInvestigation.so`.
- Read it as a more complete reference for plugin implementation details.

---

### Debugging plugins

If your plugin does not seem to be called:

- Check that the shared library path given to `-u` is correct.
- Ensure that the exported factory function name matches what `DynamicLibrary` expects (e.g. `NewUserSort`).
- Make sure that your plugin links against the same versions of the libraries as XIA2tree was built with.
- Add simple debug logging or counters inside `FillEvent` and `Flush` to confirm they are called.

Because plugins run in the same process as XIA2tree, make sure to handle errors defensively inside your plugin to avoid crashing the main program.

