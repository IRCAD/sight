# sight::core::module::viz::scene3d::test

This module is only used to create and destroy `Ogre::Root` only once in unit tests like `sight::viz::scene3d::utest`.

This is the most elegant way we found to achieve this. There are indeed four solutions:
- create and destroy the `Ogre::Root` at each test. That was our initial solution, however `xvfb-run` randomly fails the first time we destroy and re-create a `Ogre::Root`.
- create and destroy the `Ogre::Root` in a static singleton. That crashes, because `Ogre::Root` fails to destroy properly after the `main()` function.
- create and destroy the `Ogre::Root` in an initialize/uninitialize function inside the `main` function. We do not have such a system today.
- create and destroy the `Ogre::Root` in a plugin, which somehow mimics the previous solution, but we already have the system in place.
  