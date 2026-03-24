# The Cuttlefish Release Process

Here is the exact step-by-step workflow for pushing a new update to your users!

### 1. Code & Commit
1. Code your new feature on your `develop` branch (or `MakingElectronApp`).
2. Test it locally using `npm run electron:dev`.
3. Commit everything:
   ```bash
   git add .
   git commit -m "Added my new feature"
   ```

### 2. Prepare the Update
1. Open `control-dashboard/package.json`.
2. Find `"version": "1.0.0"` and bump it up (e.g. `"1.0.1"`).
3. Commit this version bump to Git:
   ```bash
   git add package.json
   git commit -m "Bumped to v1.0.1"
   ```

### 3. Merge to Master
Bring your finished feature out of `develop` and into the main spine of the app:
```bash
git checkout master
git merge develop
git push origin master
```

### 4. Trigger the Cloud Build
Tag the exact snapshot in time that you want to serve as the update:
```bash
git tag v1.0.1
git push origin v1.0.1
```
*(GitHub Actions will now wake up, compile your `.exe` and `.dmg`, and bundle them into a Draft Release).*

### 5. Publish the Update!
1. Wait ~3 minutes for the GitHub Action to finish building.
2. Go to **github.com/Koby1997/CuttlefishLights/releases**.
3. You will see your newly generated `v1.0.1` update marked with a gray **Draft** badge.
4. Click the Edit (pencil) icon in the top right of that box.
5. Give the update a Title and a Description so your users know what changed.
6. Scroll to the bottom and click the green **Publish release** button!

The second you hit Publish, any user who clicks **Check for Update** in their `.exe` application will instantly download it.
