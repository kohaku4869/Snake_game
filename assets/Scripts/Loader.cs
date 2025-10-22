using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
public static class Loader
{
    private static Action loaderCallbackAction;
    public enum Scene
    {
        GameScene,
        Loading,
        MainMenu
    }
    public static void Load(Scene scene)
    {
        SceneManager.LoadScene(Scene.Loading.ToString());
        loaderCallbackAction += () =>
        {
            if (Time.timeScale == 0f) Time.timeScale = 1f;
            SceneManager.LoadScene(scene.ToString());
        };
    }

    public static void LoaderCallBack()
    {
        if (loaderCallbackAction != null)
        {
            loaderCallbackAction();
            loaderCallbackAction = null;
        }
    }
}
