/* 
    ------------------- Code Monkey -------------------

    Thank you for downloading this package
    I hope you find it useful in your projects
    If you have any questions let me know
    Cheers!

               unitycodemonkey.com
    --------------------------------------------------
 */

using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameAssets : MonoBehaviour
{

    public static GameAssets instance;

    private void Awake()
    {
        instance = this;
    }
    public Sprite snakeHeadSprite;
    public Sprite snakeBodySprite;
    public Sprite foodSprite;

    // New sprites from assets/NewTexture
    public Sprite snakeHeadBlue;
    public Sprite snakeBodyBlue;
    public Sprite snakeHeadYellow;
    public Sprite snakeBodyYellow;

    public Sprite foodAppleBaseSprite;   // táo gốc (mồi)
    public Sprite foodAppleGreenSprite;  // buff
    public Sprite foodApplePinkSprite;   // buff

    public Sprite backgroundSprite;      // background mới
    public SoundAudioClip[] soundAudioClipArray;
    [Serializable]
    public class SoundAudioClip
    {
        public SoundManager.Sound sound;
        public AudioClip audioClip;
    }
}
