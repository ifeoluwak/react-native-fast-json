# react-native-fast-json

**`JSON.parse`** turns the whole file into nested JavaScript objects in the same memory as your app code. That is fine for small JSON; for **very large files** (on the order of **tens to hundreds of MB**), it often means:

- CPU stays pegged high while parsing finishes  
- The screen **freezes or stutters**  
- Memory use jumps; in bad cases the app is **killed for using too much memory**  

**This library** parses in **native** code with [simdjson](https://github.com/simdjson/simdjson) and exposes a **`JsonView`** through [Nitro Modules](https://nitro.margelo.com/)—you read the document **lazily** from native memory instead of inflating everything into one giant JavaScript tree up front.

**Rough illustration** for a **~250 MB** file (device and JSON shape will change this):

| | `JSON.parse` | `parseFile` + root `JsonView` |
| --- | --- | --- |
| **Parse / load** | Often **many seconds** on a phone (every object is allocated in JS) | Native read + parse often **~100 ms** for the same **~250 MB**—**near-instant** at UI scale; exact time depends on disk and CPU |
| **CPU** | Long spikes (often **>100%** in system monitors) | **Calmer** during native load/parse |
| **Memory (ballpark)** | **~1.2 GB** in a heavy case | **~400 MB** (the file still has to live somewhere) |

These numbers are **illustrative only**—measure your own file on real hardware before you budget.

## When to use it

- Large JSON (for example **~100–200 MB+**: catalogs, offline bundles, big dumps) where **`JSON.parse`** feels too slow, freezes the UI, or uses too much memory.
- Mostly **read-only**, **targeted** access: a few keys, paths, or scalars.

Prefer normal **`JSON.parse`** for small responses and whenever you need the full tree as plain JS objects anyway.

## Requirements

- React Native with the setup expected by Nitro (see [Nitro docs](https://nitro.margelo.com/)).
- **`react-native-nitro-modules`** as a dependency (peer).

## Installation

```sh
yarn add react-native-fast-json react-native-nitro-modules
# or
npm install react-native-fast-json react-native-nitro-modules
```

Then install iOS pods from your app root:

```sh
cd ios && pod install
```

Rebuild the native app after adding the dependency.

## Usage

```ts
import { fastJson, type JsonView } from 'react-native-fast-json';

// From a JSON string (loads into native memory; not cached by path)
const root = await fastJson.parseString(jsonString);
if (!root) return;

//Preferred - From a file path (native reads the file; result is cached per path)
const fromFile = await fastJson.parseFile('/path/to/data.json');

// Drop cached parse for that path when you are done (see Memory below)
fastJson.release('/path/to/data.json');
```

### `JsonView`

Navigate and read values without building a full JS tree up front:

| Method / property | Purpose |
|-------------------|--------|
| `getValue(key)` | Single object key (not a path). Returns `JsonView \| null`. |
| `keys()` | Keys for objects / arrays (as applicable). |
| `at(index)` | Array element by index (random access). |
| `next()` | Next array element in order (sequential iteration). Returns `null` at end. |
| `atPath('$.a.b.c')` | Simple dotted path from `$` (no `[index]` in path). |
| `atPathWithWildcard('$.items[*].id')` | Wildcard / index segments; returns `string[] \| null`. |
| `type`, `length` | Value kind and length (objects/arrays). |
| `asString()`, `asNumber()`, `asBoolean()` | Scalar coercion. |
| `asObject()` | Materialize object/array into an `AnyMap` (expensive for large values). |
| `rawJson()` | Raw JSON slice as string (can be expensive for large values). |

Example:

```ts
const meta = root.getValue('metadata');
const version = meta?.getValue('version')?.asString();
```

### Iterating large arrays with `next()`

For a root that is a JSON **array** (for example a catalog of records), prefer **`next()`** over **`at(i)` in a loop**. Each `at(i)` re-walks the array from the start, so a loop becomes roughly **O(n²)**. **`next()`** walks the array **once** in native code (**O(n)**).

`next()` only works on `JsonView` that is of type array.

```ts
const root = await fastJson.parseFile('/path/to/items.json');
if (!root) return;

const names: string[] = [];
let item = root.next();
while (item) {
  const name = item.getValue('name')?.asString();
  if (name) names.push(name);
  item = root.next(); // advance on the same array root
}

fastJson.release('/path/to/items.json');
```

**Rules while iterating with `next()`:**

- Do **not** call `at()`, `getValue()`, `atPath()`, or other methods on the **same root** mid-loop—they re-parse the document and invalidate the internal iterator.
- **`next()`** returns `null` when there are no more elements; no reset is required unless you add your own re-walk logic.
- Use **`getValue(key)`** on each **child** view to read fields (for example `item.getValue('artist')?.asString()`). **`asString()`** on the child itself only works when that element is a JSON string.

For **one-off** access by index, **`at(index)`** is still fine. For **many elements**, use **`next()`** or **`atPathWithWildcard`** when you need the same field from every item.

## Memory and caching

### How big is “big”?

Native memory for a parse is dominated by the **root buffer**, which is roughly **the JSON byte size plus simdjson padding** (think **on the order of the file size**, not “a few MB overhead”).

| Rough file size | What to expect |
|-----------------|------------------|
| **Under ~10 MB** | Usually fine to keep a root around for a screen session if you need repeated lazy access. |
| **~10–50 MB** | Still workable; treat the root as a **large native allocation** and avoid keeping multiple overlapping parses. |
| **~50–200 MB** | High impact on device RAM and OOM risk if you stack parses or retain roots in global state. Plan **`release(path)`** and avoid `rawJson` / `asObject` on the whole document. |
| **200 MB+** | Same as above, but stricter: **short-lived root**, extract what you need, then drop handles immediately (see below). |

These are ballparks, actual pressure depends on device, OS, and what else your app keeps in JavaScript memory versus native (C++) memory.

### Prefer a short-lived root: read what you need, then discard

Prefer not keeping a large root `JsonView` in React state, context, or a singleton for the whole app lifetime unless you truly need random access to that document for a long time.

A safer pattern for large files:

1. **`parseFile`** (or `parseString`) once when you need the data.
2. In the **same synchronous stretch** (or one small async function), walk the tree and **copy** primitives, small objects, or IDs into **plain JavaScript** values you actually store in state.
3. **Clear** your `JsonView` references and call **`release(path)`** for file parses so the native cache and buffer can go away.

```ts
async function loadConfig(path: string) {
  const root = await fastJson.parseFile(path);
  try {
    const version = root?.getValue('metadata')?.getValue('version')?.asString();
    const batchSize = root
      ?.atPath('$.metadata.configuration.export_settings.batch_size')
      ?.asNumber();
    return { version, batchSize }; // plain JS — safe to keep
  } finally {
    fastJson.release(path);
  }
}
```

That way you pay the large native buffer only while you extract fields, not for the entire time the user has the app open.

If you **do** need the root for a while (e.g. a deep drill-down UI over the same file), keep **one** root per path, avoid overlapping second parses of the same huge file, and still **`release`** when the user leaves the flow.

### Other rules of thumb

- **Root `JsonView`** holds the **full parsed buffer** in native memory (file size + padding for `parseFile`, string size for `parseString`).
- **`parseFile(path)`** caches the native view **by path string**. Repeated `parseFile` with the same path returns the same cached root until **`release(path)`** removes it.
- **`parseString`** does **not** use that path cache; each call allocates a new native view for that string (until the JS side drops the `JsonView`).
- **`getValue` / `at` / `next` / paths** may create **child** views that copy JSON slices into their own buffers. Holding many large subtrees can add up.
- Prefer **`next()`** over repeated **`at(i)`** when scanning most of an array, fewer native re-scans and less CPU.
- **`rawJson()`** and **`asObject()`** on very large values can allocate **large** extra memory (strings / maps). Use sparingly on big documents.

If you only need a subtree in JavaScript, you can still hold just that subtree, but while **`parseFile`** keeps the root in the path cache, the **full file buffer** stays in native memory until you **`release`** that path (and nothing else retains the view).

## API summary

| Method | Description |
|--------|-------------|
| `parseString(str)` | Parse JSON from a string. |
| `parseFile(path)` | Load and parse JSON from a filesystem path; cached by `path`. |
| `release(path)` | Remove cached parse for `path` (file cache only). |

## Contributing

- [Development workflow](CONTRIBUTING.md#development-workflow)
- [Sending a pull request](CONTRIBUTING.md#sending-a-pull-request)
- [Code of conduct](CODE_OF_CONDUCT.md)

## License

MIT

---

Made with [create-react-native-library](https://github.com/callstack/react-native-builder-bob)
