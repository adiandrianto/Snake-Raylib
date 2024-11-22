#include <raylib.h>
#include <raymath.h>
#include <deque>

int cellSize = 32;
int cellCount = 20;
int offset = 64;

double lastUpdateTime = 0;

bool gameOver = false;
bool eventTriggered(double interval)
{
	double currentTime = GetTime();

	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}

	return false;
}

bool isElementInDeque(Vector2 element, std::deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(element, deque[i]))
		{
			return true;
		}
	}
	return false;
}


class Snake
{
public: 
	std::deque<Vector2> body = { Vector2{5,5}, Vector2{4,5}, Vector2{3,5}, Vector2{2,5} };
	Vector2 direction = Vector2{1,0};
	bool addSegment = false;

	void Draw()
	{
		for (unsigned int i = 1; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;

			Rectangle rectangle = Rectangle{offset + x * (float)cellSize, offset + y * (float)cellSize, (float)cellSize, (float)cellSize};
			DrawRectangleRounded(rectangle, 0.5, 6, BLACK);
		}
	}

	void Move()
	{
		Vector2 newHead = Vector2Add(body[0], direction);

		if (newHead.x < 0) newHead.x = cellCount - 1;
		if (newHead.x >= cellCount ) newHead.x = 0;

		if (newHead.y < 0) newHead.y = cellCount  - 1;
		if (newHead.y >= cellCount ) newHead.y = 0;

		body.push_front(newHead);
	}
		

	void Update()
	{
		Move();
		if (addSegment)
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}

		for (unsigned int i = 1; i < body.size(); i++)
		{
			if (Vector2Equals(body[i], body[0]))
			{
				gameOver = true;
			}
		}
	}
};

class Food 
{
public:
	Vector2 position;
	Texture2D texture;

	Food()
	{
		Image image = LoadImage("apple.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = Vector2{6,7};
	}

	~Food()
	{
		UnloadTexture(texture);
	}

	void Draw()
	{
		DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, BLACK);
	}
};

class Game
{
public:
	Snake snake = Snake();
	Food food = Food();
	Sound eat;
	Sound gameover;
	Sound walk;

	Game()
	{
		InitAudioDevice();
		eat = LoadSound("eat.ogg");
		gameover = LoadSound("gameover.ogg");	
		walk = LoadSound("walk.ogg");
	}

	~Game()
	{
		CloseAudioDevice();
		UnloadSound(eat);
		UnloadSound(gameover);
	}

	void GameOver()
	{
		PlaySound(gameover);
	}

	void Draw()
	{
		snake.Draw();
		food.Draw();
	}

	void Update()
	{
		snake.Update();
	}

	void CheckCollisionWithFood()
	{
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = GenerateRandomPosition();
			snake.addSegment = true;
			PlaySound(eat);
		}
		else
		{
			PlaySound(walk);
		}
	}

	Vector2 GenerateRandomPosition()
	{
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);

		while (isElementInDeque(food.position, snake.body))
		{
			float x = GetRandomValue(0, cellCount - 1);
			float y = GetRandomValue(0, cellCount - 1);

			return Vector2{ x,y };
		}

		return Vector2{ x,y };
	}

};

int main() 
{
	InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Raylib");

	SetTargetFPS(30);

	Game game = Game();

	while (!WindowShouldClose())
	{
		BeginDrawing();

		if (eventTriggered(0.2))
		{
			game.Update();
			game.CheckCollisionWithFood();
		}

		if (gameOver)
		{
			game.GameOver();
			gameOver == false;
		}

		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
		{
			game.snake.direction = Vector2{ 0, -1 };
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
		{
			game.snake.direction = Vector2{ 0, 1 };
		}	
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
		{	
			game.snake.direction = Vector2{ -1, 0 };
		}	
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
		{	
			game.snake.direction = Vector2{ 1, 0 };
		}	

		ClearBackground(DARKGREEN);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5, BLACK);
		
		game.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}