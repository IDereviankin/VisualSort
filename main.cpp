#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 450
#define COLUMN_WIDTH 2
#define COLUMNS_COUNT (WINDOW_WIDTH / COLUMN_WIDTH)
#define HEIGHT_UNIT (WINDOW_HEIGHT / COLUMNS_COUNT)
#define WAIT_TIME 20

#define RED(_X) ((_X & 0xFF000000) >> 24)
#define GREEN(_X) ((_X & 0x00FF0000) >> 16)
#define BLUE(_X) ((_X & 0x0000FF00) >> 8)
#define ALPHA(_X) ((_X & 0x000000FF) >> 0)

using Swap = std::pair<size_t, size_t>;
using SortAlgorithm = std::function<std::vector<Swap>(uint32_t (&)[COLUMNS_COUNT])>;

std::vector<Swap> bubble_sort(uint32_t (&unsorted)[COLUMNS_COUNT])
{
    uint32_t arr[COLUMNS_COUNT]{};
    std::vector<Swap> swaps{};

    std::copy(unsorted, unsorted + COLUMNS_COUNT, arr);
    auto swap = [&](size_t i, size_t j)
    {
        swaps.push_back({i, j});

        auto t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    };

    for (size_t i = 0; i < COLUMNS_COUNT; ++i)
    {
        for (size_t j = 1; j < (COLUMNS_COUNT - i); ++j)
        {
            if (arr[j - 1] > arr[j])
                swap(j - 1, j);
        }
    }

    return swaps;
};

std::vector<Swap> _quick_sort(uint32_t (&unsorted)[COLUMNS_COUNT], size_t lo, size_t hi)
{
    uint32_t arr[COLUMNS_COUNT]{};
    std::vector<Swap> swaps{};

    std::copy(unsorted, unsorted + COLUMNS_COUNT, arr);
    auto swap = [&](size_t i, size_t j)
    {
        swaps.push_back({i, j});

        auto t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    };

    if(lo >= hi || lo < 0) return swaps;

    auto pivot = arr[hi];
    auto i = lo - 1;

    for(auto j = lo; j < hi; ++j) {
        if(arr[j] <= pivot) {
            i += 1;
            swap(i, j);
        }
    }

    i += 1;
    swap(i, hi);

    auto swaps1 = _quick_sort(arr, lo, i - 1);
    auto swaps2 = _quick_sort(arr, i + 1, hi);
    swaps.insert(swaps.end(), swaps1.begin(), swaps1.end());
    swaps.insert(swaps.end(), swaps2.begin(), swaps2.end());
    
    return swaps;
};

std::vector<Swap> quick_sort(uint32_t (&unsorted)[COLUMNS_COUNT]) {
    return _quick_sort(unsorted, 0, COLUMNS_COUNT - 1);
};

struct VisualSortApp
{
private:
    bool wanna_quit = false;
    uint32_t columns[COLUMNS_COUNT];
    std::vector<Swap> swaps{};
    SDL_Window *win;
    SDL_Renderer *rend;

    void setColor(uint32_t rgba)
    {
        SDL_SetRenderDrawColor(rend, RED(rgba), GREEN(rgba), BLUE(rgba), ALPHA(rgba));
    }

    void drawRect(int x, int y, int w, int h)
    {
        SDL_Rect r = {.x = x, .y = y, .w = w, .h = h};
        SDL_RenderFillRect(rend, &r);
    }

    void drawRect(SDL_Rect r)
    {
        SDL_RenderFillRect(rend, &r);
    }

    void clearScreen()
    {
        SDL_RenderClear(rend);
    }

    void updateScreen()
    {
        SDL_RenderPresent(rend);
    }

public:
    void startMainloop()
    {
        size_t counter = 0;
        SDL_Event ev;

        while (true)
        {
            if (counter < swaps.size())
            {
                auto [a, b] = swaps[counter];

                auto t = columns[a];
                columns[a] = columns[b];
                columns[b] = t;

                counter += 1;
            }

            setColor(0x000000FF);
            clearScreen();

            setColor(0xFFFFFFFF);

            int x = 0;
            for (auto column : columns)
            {
                drawRect(x, WINDOW_HEIGHT - (column), COLUMN_WIDTH, column);
                x += COLUMN_WIDTH;
            }

            updateScreen();

            SDL_WaitEventTimeout(&ev, WAIT_TIME);
            switch (ev.type)
            {
            case SDL_QUIT:
                wanna_quit = true;
                break;
            }

            if (wanna_quit)
                break;
        }
    }

    VisualSortApp(SortAlgorithm sort)
    {
        if (SDL_Init(SDL_INIT_EVERYTHING))
        {
            exit(1);
        };

        uint32_t counter = HEIGHT_UNIT;
        for (auto &column : columns)
        {
            column = counter;
            counter += HEIGHT_UNIT;
        }

        std::random_shuffle(columns, columns + COLUMNS_COUNT);

        swaps = sort(columns);

        win = SDL_CreateWindow("VisualSort",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               WINDOW_WIDTH,
                               WINDOW_HEIGHT, 0);
        rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    }

    ~VisualSortApp()
    {
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
    }
};

int main()
{
    // std::function f = bubble_sort;
    VisualSortApp app{SortAlgorithm(quick_sort)};
    app.startMainloop();
}