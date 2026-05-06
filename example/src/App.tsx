import { Text, View, StyleSheet, Button } from 'react-native';
import { fastJson } from 'react-native-fast-json';
import { rnNitroCache } from 'react-native-nitro-cache';

// const jsonUrl =
//   'https://microsoftedge.github.io/Demos/json-dummy-data/5MB-min.json';
const largeJson100mbUrl =
  'https://elon1.pcloud.com/cBZt3PehcZ6Emo0A7ZZZH86B5kZ2ZZhE0ZkZP5cMJZvpZMRZqpZw4ZapZxVZFQZURZO9Zv8ZwpZN8ZXFZEgZcqKiZR8rtBYhnGy7zJ01A8Hbj45tkObLy/data_100mb.json';

export default function App() {
  // useEffect(() => {
  //   // fastJson
  //   //   .parse(
  //   //     '{ "name": "John", "age": 30, "address": { "street": "123 Main St", "city": "Anytown", "state": "CA", "zip": "12345" } }'
  //   //   )
  //   //   .then((result) => {
  //   //     // console.log(result?.getValue('name'));
  //   //     console.log(result?.getValue('addressss'));
  //   //     // console.log(result?.getValue('address')?.has('streets'));
  //   //     console.log(result?.getValue('address')?.toJson());
  //   //     console.log(result?.getValue('address')?.length);
  //   //     console.log(result?.getValue('address')?.type);
  //   //     console.log(result?.getValue('address')?.keys());
  //   //   });
  //   let start = performance.now();
  //   // console.log(JSON.parse(JSON.stringify(exampleJson)));
  //   fastJson.parse(JSON.stringify(exampleJson)).then((result) => {
  //     // console.log(result?.getValue('items'));
  //     let end = performance.now();
  //     console.log(end - start);
  //   });
  //   // let end = performance.now();
  //   // console.log(end - start);
  // }, []);

  return (
    <View style={styles.container}>
      <Text>Result</Text>

      <Button
        title="Download JSON to Disk"
        onPress={() => {
          rnNitroCache
            .getOrFetch(largeJson100mbUrl)
            .then((result) => {
              console.log('Downloaded JSON to Disk: ', result);
            })
            .catch((error) => {
              console.log('errored download', error);
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
          // let start = Date.now();
          // JSON.parse(JSON.stringify(exampleJson));
          // let end = Date.now();
          // console.log(end - start);
        }}
      />

      <Button
        title="FastJSON Parse"
        onPress={async () => {
          const filePath = await rnNitroCache.get(largeJson100mbUrl);
          // console.log('filePath: ', filePath);
          const start = Date.now();
          fastJson.parse({ uri: filePath?.url ?? '' }).then((result) => {
            // let end = Date.now();
            // console.log(end - start);
            // console.log(result?.atPathWithWildcard('$.events[*].subTopicIds'));
            // console.log(
            //   result?.atPathWithWildcard('$.performances[0].prices[0]')
            // );
            // console.log(result?.getValue('metadata')?.rawJson());
            // console.log(result);
            const end = Date.now();
            console.log(end - start);
            // console.log(result?.at(2));

            const start2 = Date.now();
            // console.log(result?.atPath("$.metadata.configuration")?.rawJson());
            // console.log(result?.getValue("metadata")?.rawJson());
            console.log(result?.atPath('$.metadata.statistics')?.rawJson());
            const end2 = Date.now();
            console.log('-p-p-p', end2 - start2);
            console.log('-2.2.2', end2 - start);
          });
        }}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
});
