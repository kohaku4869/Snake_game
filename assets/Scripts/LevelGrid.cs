using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using CodeMonkey;
using CodeMonkey.Utils;

public class LevelGrid
{
    private Vector2Int foodGridPosition;
    private int width, height;
    private GameObject foodGameObject;
    private Snake snake;
    private bool lastWasBuff;

    public LevelGrid(int width, int height)
    {
        this.width = width;
        this.height = height;
    }
    public void Setup(Snake snake)
    {
        this.snake = snake;
        SpawnFood();
    }
    private void SpawnFood()
    {
        do
        {
            foodGridPosition = new Vector2Int(Random.Range(0, width), Random.Range(0, height));
        } while (snake.GetFullSnakeGridPositionList().IndexOf(foodGridPosition) != -1);
        foodGameObject = new GameObject("Food", typeof(SpriteRenderer));
        var sr = foodGameObject.GetComponent<SpriteRenderer>();
        // 70% mồi thường (táo gốc), 30% buff (táo màu khác)
        bool spawnBuff = Random.value < 0.3f;
        if (spawnBuff)
        {
            // chọn một trong các táo buff có sẵn
            Sprite[] buffs = new Sprite[] { GameAssets.instance.foodAppleGreenSprite, GameAssets.instance.foodApplePinkSprite };
            // fallback về táo gốc nếu thiếu sprite
            Sprite chosen = null;
            foreach (var s in buffs)
            {
                if (s != null)
                {
                    chosen = s;
                    break;
                }
            }
            sr.sprite = chosen != null ? chosen : (GameAssets.instance.foodAppleBaseSprite != null ? GameAssets.instance.foodAppleBaseSprite : GameAssets.instance.foodSprite);
        }
        else
        {
            sr.sprite = GameAssets.instance.foodAppleBaseSprite != null ? GameAssets.instance.foodAppleBaseSprite : GameAssets.instance.foodSprite;
        }
        foodGameObject.transform.position = new Vector2(foodGridPosition.x, foodGridPosition.y);
    }

    public bool TrySnakeEatFood(Vector2Int snakeGridPosition)
    {
        if (snakeGridPosition == foodGridPosition)
        {
            Object.Destroy(foodGameObject);
            SpawnFood();
            CMDebug.TextPopupMouse("Snake Ate Food");
            GameHandler.AddScore();
            return true;
        }
        else
        {
            return false;
        }
    }
    public Vector2Int ValidateGridPosition(Vector2Int gridPosition)
    {
        if (gridPosition.x < 0)
        {
            gridPosition.x = width - 1;
        }
        if (gridPosition.x > width - 1)
        {
            gridPosition.x = 0;
        }
        if (gridPosition.y < 0)
        {
            gridPosition.y = height - 1;
        }
        if (gridPosition.y >  height -1)
        {
            gridPosition.y = 0;
        }
        return gridPosition;
    }
}
