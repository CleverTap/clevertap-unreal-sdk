# CleverTap Product Experiences (PE) Variables — Unreal SDK *.* 

PE Variables let your CleverTap dashboard operators change values inside your game or app at runtime — without a code update or new build. You define a variable in code with a default value; an operator can override that value from the dashboard; the next time the app calls `FetchVariables()` it picks up the new value.

---

## Table of Contents

1. [How It Works](#how-it-works)
2. [Quick Start](#quick-start)
3. [Step-by-Step Guide](#step-by-step-guide)
4. [Variable Types Reference](#variable-types-reference)
5. [API Reference](#api-reference)
6. [Delegates](#delegates)
7. [Dashboard Workflow](#dashboard-workflow)
8. [Common Issues](#common-issues)

---

## How It Works

```
Your Code                         CleverTap Dashboard
─────────                         ───────────────────
DefineIntVariable("lives", 3)
SyncVariables()           ──────► Registers name / type / default
                                  Operator sets override → 5
FetchVariables()          ◄─────  Returns override value
GetIntVariable("lives", 3)  →  5
```

- **Define** — registers a variable's name, type, and code default with the SDK. Must happen before any sync or fetch.
- **Sync** — uploads variable definitions to the dashboard (development / testing only). Makes variables visible to operators so they can set overrides. Has no effect on values inside the running app.
- **Fetch** — downloads current server-side overrides from the dashboard. This is what makes PE Variables useful in production.
- **Get** — reads the current effective value: server override if one exists, otherwise the code default.

---

## Quick Start

```cpp
// 1. Get the CleverTap instance
UCleverTapInstance& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

// 2. Bind delegates FIRST — so you never miss a callback
CT.OnVariablesFetched.AddDynamic(this, &UMyClass::OnFetched);
CT.OnVariablesChanged.AddDynamic(this, &UMyClass::OnChanged);

// 3. Define variables (call once at startup)
CT.DefineIntVariable(TEXT("spawn_count"), 10);
CT.DefineStringVariable(TEXT("welcome_msg"), TEXT("Hello!"));
CT.DefineBoolVariable(TEXT("feature_enabled"), false);

// 4. Sync to dashboard (DEV ONLY — lets operators see and override variables)
CT.SyncVariables();

// 5. Fetch server overrides
CT.FetchVariables();

// 6. Read values inside OnVariablesChanged
void UMyClass::OnChanged()
{
    int32 Count = CT.GetIntVariable(TEXT("spawn_count"), 10);
    FString Msg  = CT.GetStringVariable(TEXT("welcome_msg"), TEXT("Hello!"));
    bool    Flag = CT.GetBoolVariable(TEXT("feature_enabled"), false);
}
```

---

## Step-by-Step Guide

### Step 1 — Bind Delegates (before defining)

Bind `OnVariablesFetched` and `OnVariablesChanged` **before** calling `DefineXxxVariable`. If you bind after fetching you may miss the first callback.

```cpp
CT.OnVariablesFetched.AddDynamic(this, &UMyClass::HandleFetched);
CT.OnVariablesChanged.AddDynamic(this, &UMyClass::HandleChanged);
```

`OnVariablesFetched(bool bSuccess)` fires when the HTTP request completes.  
`OnVariablesChanged` fires when one or more values actually changed (after a fetch or server push).

---

### Step 2 — Define Variables

Call `DefineXxxVariable` once at startup — typically in `BeginPlay` of your first game mode or a subsystem initializer. Variables are **immutable after first definition**: calling `Define` a second time with a different default is a no-op. Change the default in code and rebuild to update it.

```cpp
// Scalar types
CT.DefineStringVariable (TEXT("var_name"), TEXT("default"));
CT.DefineIntVariable    (TEXT("var_name"), 10);
CT.DefineInt64Variable  (TEXT("var_name"), 1000000000LL);   // C++ only, not Blueprint
CT.DefineFloatVariable  (TEXT("var_name"), 1.5f);
CT.DefineDoubleVariable (TEXT("var_name"), 3.14159265358979); // C++ only, not Blueprint
CT.DefineBoolVariable   (TEXT("var_name"), true);

// Map (group) type — key/value pairs
CT.DefineStringMapVariable(TEXT("var_name"),
    TMap<FString,FString>{ {TEXT("key1"), TEXT("val1")}, {TEXT("key2"), TEXT("val2")} });

// File type — value is a local path populated after FetchVariables downloads the file
CT.DefineFileVariable(TEXT("var_name"));
```

**Naming tip:** Use a unique prefix (e.g. `mygame_`) to avoid collisions with other projects or developers sharing the same CleverTap account. Variable names are global per account.

---

### Step 3 — Sync to Dashboard (development / testing only)

`SyncVariables()` uploads your variable definitions (name, type, default) to the CleverTap dashboard so operators can see them and set overrides.

```cpp
CT.SyncVariables();
```

- Call this **after** `DefineXxxVariable` and only from **development / testing builds**.
- Do **not** ship `SyncVariables()` calls in production — there is no benefit and it generates unnecessary server traffic.
- After syncing, open **CleverTap Dashboard → Product Experiences → Variables** to see your variables and set overrides.

> **Two requirements for `SyncVariables` to work on both platforms:**
>
> **1. Test profile must be enabled on the CleverTap dashboard.**
> Go to **CleverTap Dashboard → Settings → (your app) → Test Profiles** and ensure the device or the account's test mode is active. This is a backend gate — the server rejects sync requests from accounts that do not have test/development mode enabled. `FetchVariables()` does not have this restriction and works regardless of test profile status.
>
> **2. The build must be a Development (not Shipping) build.**
>
> - **Android** — The SDK checks `ApplicationInfo.FLAG_DEBUGGABLE` at runtime. Unreal Engine sets `debuggable=true` automatically for **Debug** and **Development** builds. **Shipping** builds have `debuggable=false` and `SyncVariables()` silently does nothing — no request is sent, no error is reported.
> - **iOS** — Development build is also required. The SDK enforces this server-side as part of the test profile + development mode pairing.
>
> `FetchVariables()` has **no build-mode or test-profile requirement** — it works in any build including Shipping.

---

### Step 4 — Fetch Server Overrides

```cpp
CT.FetchVariables();
```

Call this at app start (or on demand, e.g. after login) to pull the latest operator-set values from the server. `OnVariablesFetched` fires on completion; if any values changed, `OnVariablesChanged` fires immediately after.

```cpp
void UMyClass::HandleFetched(bool bSuccess)
{
    if (bSuccess)
    {
        // Values are now up to date — read them in HandleChanged
        UE_LOG(LogGame, Log, TEXT("PE Variables fetched successfully"));
    }
    else
    {
        UE_LOG(LogGame, Warning, TEXT("PE Variables fetch failed — using defaults"));
    }
}

void UMyClass::HandleChanged()
{
    // Read all variable values here
    int32 Lives = CT.GetIntVariable(TEXT("lives"), 3);
    // ... apply to game state
}
```

---

### Step 5 — Read Values

Always read inside `OnVariablesChanged` (or after `OnVariablesFetched` with `bSuccess=true`). If you read before a successful fetch, you get the code default.

```cpp
FString  S = CT.GetStringVariable   (TEXT("var_name"), TEXT("default"));
int32    I = CT.GetIntVariable      (TEXT("var_name"), 0);
int64    L = CT.GetInt64Variable    (TEXT("var_name"), 0LL);         // C++ only
float    F = CT.GetFloatVariable    (TEXT("var_name"), 0.0f);
double   D = CT.GetDoubleVariable   (TEXT("var_name"), 0.0);         // C++ only
bool     B = CT.GetBoolVariable     (TEXT("var_name"), false);
TMap<FString,FString> M = CT.GetStringMapVariable(TEXT("var_name"), {});
FString  P = CT.GetFileVariablePath (TEXT("var_name"));  // empty until downloaded
```

Every `GetXxx` call takes a `DefaultValue` argument — this is returned if the variable was never defined, the fetch has not completed yet, or the value is the wrong type. Always pass a sensible default that keeps your game functional.

---

## Variable Types Reference

| C++ Type | Define Method | Get Method | Notes |
|---|---|---|---|
| `FString` | `DefineStringVariable` | `GetStringVariable` | |
| `int32` | `DefineIntVariable` | `GetIntVariable` | Blueprint callable |
| `int64` | `DefineInt64Variable` | `GetInt64Variable` | C++ only |
| `float` | `DefineFloatVariable` | `GetFloatVariable` | Stored as double internally; see precision note below |
| `double` | `DefineDoubleVariable` | `GetDoubleVariable` | C++ only; preferred over float for display |
| `bool` | `DefineBoolVariable` | `GetBoolVariable` | Blueprint callable |
| `TMap<FString,FString>` | `DefineStringMapVariable` | `GetStringMapVariable` | "Group" type on the dashboard |
| File path | `DefineFileVariable` | `GetFileVariablePath` | Returns local path after download; empty until ready |

> **Float precision note:** Float variables are stored as 64-bit doubles in both the Android and iOS SDKs. `GetFloatVariable()` casts the double to `float` at the boundary, which can introduce small representation noise (e.g. server value `67.3` → `67.300003f`). For display purposes, use `GetDoubleVariable()` on a float-type variable and format with `%.8g`.

> **short / byte:** There are no 16-bit or 8-bit variable types on the CleverTap backend. Use `DefineIntVariable` for these — the values are stored as 32-bit integers.

---

## API Reference

All methods are on `UCleverTapInstance`. Obtain the instance via:

```cpp
UCleverTapInstance& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
```

### Define Methods

| Method | Description |
|---|---|
| `DefineStringVariable(Name, Default)` | Register a string variable. |
| `DefineIntVariable(Name, Default)` | Register a 32-bit integer variable. |
| `DefineInt64Variable(Name, Default)` | Register a 64-bit integer variable (C++ only). |
| `DefineFloatVariable(Name, Default)` | Register a float variable (stored as double internally). |
| `DefineDoubleVariable(Name, Default)` | Register a 64-bit double variable (C++ only). |
| `DefineBoolVariable(Name, Default)` | Register a boolean variable. |
| `DefineStringMapVariable(Name, Default)` | Register a `TMap<FString,FString>` group variable. |
| `DefineFileVariable(Name)` | Register a file variable — no default; value is a local path populated after download. |

### Action Methods

| Method | When to call | Description |
|---|---|---|
| `SyncVariables()` | After `Define*`, dev builds only | Uploads variable definitions to the dashboard so operators can see and override them. No-op for values inside the app. |
| `FetchVariables()` | App start, after login, or on demand | Downloads server-side overrides. Fires `OnVariablesFetched` on completion and `OnVariablesChanged` if values changed. |

### Get Methods

| Method | Description |
|---|---|
| `GetStringVariable(Name, Default)` | Returns the current string value (server override or code default). |
| `GetIntVariable(Name, Default)` | Returns the current `int32` value. |
| `GetInt64Variable(Name, Default)` | Returns the current `int64` value (C++ only). |
| `GetFloatVariable(Name, Default)` | Returns the current `float` value (precision note above). |
| `GetDoubleVariable(Name, Default)` | Returns the current `double` value (C++ only). |
| `GetBoolVariable(Name, Default)` | Returns the current `bool` value. |
| `GetStringMapVariable(Name, Default)` | Returns the current `TMap<FString,FString>` value. |
| `GetFileVariablePath(Name)` | Returns the local file path of a downloaded file variable; empty string if not yet downloaded. |

### Delegates

| Delegate | Signature | Fires when |
|---|---|---|
| `OnVariablesFetched` | `void(bool bSuccess)` | `FetchVariables()` HTTP call completes. `bSuccess=false` means a timeout or network error — local defaults remain in effect. |
| `OnVariablesChanged` | `void()` | One or more variable values changed after a fetch or server push. This is the canonical place to read updated values. |

---

## Dashboard Workflow

### Making a Variable Overridable

1. Call `DefineXxxVariable` in code, then `SyncVariables()` from a dev build.
2. Open **CleverTap Dashboard → Product Experiences → Variables**.
3. Your variable appears with its name, type, and code default.
4. Click the variable → set an override value → **Save Draft** → **Publish**.
5. Next time the app calls `FetchVariables()`, it receives the override. `OnVariablesChanged` fires and your code reads the new value.

### Clearing an Override

Go to **Dashboard → Product Experiences → Variables** → find the variable → **Reset** or delete the override. The next fetch will return the code default.

### Clearing a Blocked Draft (error: "Draft of another user already exists")

CleverTap allows only one unpublished draft per account at a time. If sync fails silently or a variable never appears after sync, go to **Dashboard → Product Experiences → Variables** → look for a **Publish** or **Discard Draft** banner → resolve it. Then sync again from the app.

---

## Common Issues

### Variables not appearing on the dashboard after Sync

- Did you call `DefineXxxVariable` **before** `SyncVariables()`? Sync only uploads variables that have already been defined.
- Is **Test Profile enabled** on the CleverTap dashboard? This is required on both platforms — the server rejects sync requests without it.
- Is the build a **Development build** (not Shipping)? Both Android and iOS require development mode. On Android the SDK checks `FLAG_DEBUGGABLE` at runtime and silently no-ops on Shipping builds. Use Development build for syncing.
- Is there an existing open draft on the account? See the **Clearing a Blocked Draft** section above.
- Make sure you are looking at the **correct section** of the dashboard. Variables synced from this SDK appear in the **Production** section.

### `FetchVariables()` returns unexpected values (not the code default)

This is correct behaviour — the dashboard has a server-side override set from a previous test session. `FetchVariables()` is working as intended. To reset: Dashboard → Product Experiences → Variables → reset the individual variable's override value.

### `OnVariablesChanged` never fires after `FetchVariables()`

`OnVariablesChanged` only fires when values **actually change** relative to the last fetch. If the server returns the same values as last time (including code defaults), `OnVariablesFetched` fires but `OnVariablesChanged` does not. Always handle both: do your UI refresh in `OnVariablesChanged` and also call it directly inside `OnVariablesFetched` as a fallback if `bSuccess == true`.

### Variable names conflict with another developer / project

Variable names are **global per CleverTap account**. If a variable with the same name was defined in a different project or by another developer, the SDK returns the old definition (variable definitions are immutable once registered on the server). Always use a unique prefix: e.g. `mygame_lives` rather than just `lives`.

### File variable path is empty

`GetFileVariablePath()` returns an empty string until `FetchVariables()` has completed **and** the SDK has finished downloading the file. Wait for `OnVariablesChanged` (or `onVariablesChangedAndNoDownloadsPending` if you need to be sure the download is done) before accessing the path.

### Blueprint — `int64` and `double` are not available

The Unreal Blueprint system does not support `int64` or `double` as delegate or function parameters. Use `DefineIntVariable` / `GetIntVariable` (int32) and `DefineFloatVariable` / `GetFloatVariable` (float) from Blueprint. For full 64-bit precision, use C++.

---

## Complete Example

```cpp
// MyGameMode.h
UCLASS()
class AMyGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnVariablesFetched(bool bSuccess);

    UFUNCTION()
    void OnVariablesChanged();

    void ApplyVariables();
};

// MyGameMode.cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    UCleverTapInstance& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

    // 1. Bind delegates first
    CT.OnVariablesFetched.AddDynamic(this, &AMyGameMode::OnVariablesFetched);
    CT.OnVariablesChanged.AddDynamic(this, &AMyGameMode::OnVariablesChanged);

    // 2. Define variables
    CT.DefineIntVariable   (TEXT("mygame_lives"),          3);
    CT.DefineFloatVariable (TEXT("mygame_speed"),          5.0f);
    CT.DefineBoolVariable  (TEXT("mygame_new_ui"),         false);
    CT.DefineStringVariable(TEXT("mygame_welcome"),        TEXT("Welcome back!"));
    CT.DefineStringMapVariable(TEXT("mygame_theme"),
        TMap<FString,FString>{ {TEXT("bg"), TEXT("#000000")}, {TEXT("fg"), TEXT("#FFFFFF")} });

    // 3. Sync to dashboard — REMOVE THIS LINE IN PRODUCTION
    CT.SyncVariables();

    // 4. Fetch server overrides
    CT.FetchVariables();
}

void AMyGameMode::OnVariablesFetched(bool bSuccess)
{
    if (bSuccess)
    {
        ApplyVariables(); // also apply here in case OnVariablesChanged doesn't fire
    }
}

void AMyGameMode::OnVariablesChanged()
{
    ApplyVariables();
}

void AMyGameMode::ApplyVariables()
{
    UCleverTapInstance& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

    int32  Lives   = CT.GetIntVariable   (TEXT("mygame_lives"),   3);
    float  Speed   = CT.GetFloatVariable (TEXT("mygame_speed"),   5.0f);
    bool   NewUI   = CT.GetBoolVariable  (TEXT("mygame_new_ui"),  false);
    FString Welcome = CT.GetStringVariable(TEXT("mygame_welcome"), TEXT("Welcome back!"));

    // Apply values to your game
    UE_LOG(LogGame, Log, TEXT("PE Variables applied: lives=%d speed=%.2f new_ui=%s"),
        Lives, Speed, NewUI ? TEXT("ON") : TEXT("OFF"));
}
```
