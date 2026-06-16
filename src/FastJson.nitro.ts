import type { AnyMap, HybridObject } from 'react-native-nitro-modules';

export interface JsonView extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  /**
   * Returns the raw JSON string of the JSON view.
   * @returns The raw JSON string of the JSON view.
   */
  rawJson(): string;
  /**
   * Returns the value of the key in the JSON view.
   * @param key The key to get the value of. Must be a single key, not a path.
   * @returns The value of the key in the JSON view.
   */
  getValue(key: string): JsonView | null;
  /**
   * Returns all the keys or fields of an object or array.
   * Only for objects or arrays.
   * @returns The keys of the JSON view.
   */
  keys(): string[];
  /**
   * Only for arrays. Returns the value of the index of an array.
   * @param index The index to get the value from an array.
   * @returns The value of the index in the array.
   */
  at(index: number): JsonView | null;
  /**
   * Only for arrays. Returns the next value in the array.
   * @returns The next value in the array.
   */
  next(): JsonView | null;
  /**
   * Returns all the values of the path specified.
   * @param path simple path e.g $.key1.key2.key3. Indexing is not supported.
   * @returns Returns a JsonView.
   */
  atPath(path: string /* $.key1.key2.key3 */): JsonView | null;
  /**
   * For dynamic retrieval of nested values with wildcards.
   * Supports indexing with [*] or [number].
   * @param path Must begin with $. e.g $.key1[*].key2[1]
   * @returns all the values of the path specified.
   */
  atPathWithWildcard(path: string /* $.key1[*].key2[1] */): string[] | null;
  /**
   * Type of the value. e.g string, number, boolean, object, array.
   */
  type: string;
  /**
   * Only for objects or arrays.
   * @returns The length of the value. 0 for other types.
   */
  length: number;
  /**
   * Returns the value as a string.
   * @returns The value as a string.
   */
  asString(): string;
  asNumber(): number;
  asBoolean(): boolean;
  /**
   * For casting to JS objects or arrays.
   * Only if element is an object or array.
   * @returns `{data: Record<string, any> | any[]}`.
   */
  asObject(): AnyMap;
}

export interface FastJson extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  /**
   * Parses a JSON string and returns a Promise that resolves to a JsonView.
   * Not ideal, prefer parseFile or use simple JSON.parse if possible.
   * @param str The JSON string.
   * @returns A Promise that resolves to a JsonView or null. Response is not cached.
   */
  parseString(str: string): Promise<JsonView | null>;
  /**
   * Parses a JSON file and returns a Promise that resolves to a JsonView.
   * No check is done if invalid path is provided. Error handling is left to the caller.
   * @param path The path to the JSON file.
   * @returns A Promise that resolves to a JsonView or null. Response is cached for future calls.
   */
  parseFile(path: string): Promise<JsonView | null>;
  release(source: string): void;
}
