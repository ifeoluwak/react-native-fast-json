import { useState } from 'react';
import {
  Text,
  View,
  StyleSheet,
  Button,
  ScrollView,
  ActivityIndicator,
} from 'react-native';
import { fastJson, type JsonView } from 'react-native-fast-json';
import { rnNitroCache } from 'react-native-nitro-cache';

const jsonUrl_250mb =
  'https://github.com/antonmedv/json-examples/raw/refs/heads/master/data_250mb.json?download=';

export default function App() {
  const [jsonView, setJsonView] = useState<JsonView | null>(null);
  const [loading, setLoading] = useState(false);
  const [startTime, setStartTime] = useState(0);
  const [endTime, setEndTime] = useState(0);

  const fetchJson = async () => {
    try {
      setLoading(true);
      const response = await fetch(jsonUrl_250mb);
      if (!response.ok) {
        throw new Error('Failed to fetch JSON file');
      }
      setStartTime(Date.now());
      const data = await response.json();
      setEndTime(Date.now());
      console.log('data: ', data?.metadata);
    } catch (error) {
      console.log('error: ', error);
    } finally {
      setLoading(false);
    }
  };

  return (
    <ScrollView contentContainerStyle={{ flexGrow: 1 }}>
      {loading ? (
        <View style={styles.container}>
          <ActivityIndicator size="large" color="#0000ff" />
          <Text>Downloading JSON file...</Text>
        </View>
      ) : (
        <View style={styles.container}>
          <Text>Result</Text>

          <Button
            title="Download JSON to Disk"
            onPress={() => {
              setLoading(true);
              rnNitroCache
                .getOrFetch(jsonUrl_250mb)
                .then((result) => {
                  console.log('Downloaded JSON to Disk: ', result);
                })
                .catch((error) => {
                  console.log('errored download', error);
                })
                .finally(() => {
                  setLoading(false);
                });
            }}
          />
          <Button
            title="Check Cache for JSON"
            onPress={() => {
              rnNitroCache.getEntries().then((result) => {
                console.log('Cache entries: ', result);
              });
            }}
          />
          <Button
            title="Clear Cache"
            onPress={() => {
              rnNitroCache.clear().then((result) => {
                console.log('Cache entries: ', result);
              });
            }}
          />

          <Button
            title="JS Parse"
            onPress={async () => {
              fetchJson();
            }}
          />

          <Button
            title="FastJSON Parse"
            onPress={async () => {
              const filePath = await rnNitroCache.get(jsonUrl_250mb);
              // console.log('filePath: ', filePath);
              setStartTime(Date.now());
              fastJson.parseFile(filePath?.url ?? '').then((result) => {
                setEndTime(Date.now());
                setJsonView(result?.getValue('metadata') ?? null);
              });
            }}
          />
          <Button
            title="FastJSON Parse string"
            onPress={async () => {
              setStartTime(Date.now());
              fastJson
                .parseString(
                  '{"squadName":"Super hero squad","homeTown":"Metro City","formed":2016,"secretBase":"Super tower","active":true,"members":[{"name":"Molecule Man","age":29,"secretIdentity":"Dan Jukes","powers":["Radiation resistance","Turning tiny","Radiation blast"]},{"name":"Madame Uppercut","age":39,"secretIdentity":"Jane Wilson","powers":["Million tonne punch","Damage resistance","Superhuman reflexes"]},{"name":"Eternal Flame","age":1000000,"secretIdentity":"Unknown","powers":["Immortality","Heat Immunity","Inferno","Teleportation","Interdimensional travel"]}]}'
                )
                .then((result) => {
                  setEndTime(Date.now());
                  console.log('result: ', result?.keys());
                });
            }}
          />

          <Button
            title="Release All"
            onPress={async () => {
              const filePath = await rnNitroCache.get(jsonUrl_250mb);
              fastJson.release(filePath?.url ?? '');
            }}
          />

          <Button
            title="Metadata"
            onPress={async () => {
              let ans = jsonView?.atPath('$.statistics')?.asObject();
              console.log('ans: ', ans);
            }}
          />
          <Button
            title="Users"
            onPress={async () => {
              let ans = jsonView?.getValue('version')?.type;
              console.log('users: ', ans);
            }}
          />

          <View>
            <Text>Time taken to parse: {endTime - startTime}ms</Text>
            {/* <Text>{jsonView?.type}</Text>
            <Text>{jsonView?.keys().join(', ')}</Text> */}
            {/* <Text>{jsonView?.keys().join(', ')}</Text> */}
            {/* <Text>{jsonView?.type}</Text>
          <Text>{jsonView?.length}</Text>
          <Text>{jsonView?.asString()}</Text>
          <Text>{jsonView?.asNumber()}</Text>
          <Text>{jsonView?.asBoolean()}</Text> */}
            {/* <Text>{jsonView?.atPath('$.configuration')?.rawJson()}</Text> */}
          </View>
        </View>
      )}
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    gap: 10,
  },
});
