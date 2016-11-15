import mongo from './mongo';

export default data => {
  return mongo.then(db => {
    const collection = db.collection('things');
    collection.updateOne({
      user: data.user
    },
      { $push: {
        things:
          {
            name: data.thingName,
            type: data.type
          }
      }
    },
      {
        upsert: true
      })
  })
}
