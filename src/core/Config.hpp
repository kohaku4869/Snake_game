/**
 * @file Config.hpp
 * @brief Cấu hình game - chứa tất cả các hằng số và thông số của game
 * @author tuanq
 * @date 10/09/2025
 */

#pragma once
#include <cstdint>

namespace cfg {
    // ==== CẤU HÌNH LƯỚI VÀ MÀN HÌNH ====
    constexpr int   GRID_W = 40;              // Số ô lưới theo chiều ngang
    constexpr int   GRID_H = 30;              // Số ô lưới theo chiều dọc
    constexpr int   CELL   = 20;              // Kích thước mỗi ô lưới (pixel)
    constexpr int   FPS    = 120;              // FPS của game - tăng để rắn chuyển động mượt mà hơn
    constexpr int   ROUND_TIME_S = 180;       // Thời gian mỗi round (giây)

    // ==== CẤU HÌNH RẮN ====
    constexpr int   SNAKE_INIT_LEN = 6;       // Độ dài ban đầu của rắn
    constexpr float SNAKE_SPEED_CPS = 10.f;    // Tốc độ rắn (ô/giây)
    constexpr float SPEED_BOOST_FACTOR = 1.3f; // Hệ số tăng tốc khi boost

    // ==== CẤU HÌNH ĐẠN ====
    constexpr float BULLET_SPEED_CPS = 25.f;  // Tốc độ đạn (ô/giây)
    constexpr int   SHOOT_COOLDOWN_MS = 300;   // Thời gian chờ giữa các lần bắn (ms)

    // ==== CẤU HÌNH ITEMS ====
    constexpr int   APPLE_SPAWN_EVERY_MS = 2000;  // Táo xuất hiện mỗi 2 giây
    constexpr int   MAX_APPLES_ON_SCREEN = 3;      // Tối đa 3 táo trên màn hình
    constexpr int   BUFF_SPAWN_EVERY_MS = 10000;   // Buff xuất hiện mỗi 10 giây
    constexpr int   MAX_BUFFS_ON_SCREEN = 1;       // Tối đa 1 buff trên màn hình
    constexpr int   BUFF_LIFETIME_MS = 10000;      // Buff biến mất sau 10s nếu không ai ăn

    // ==== CẤU HÌNH SÁT THƯƠNG ====
    constexpr int   HIT_HEAD_PENALTY = 2;     // Sát thương khi trúng đầu
    constexpr int   HIT_BODY_PENALTY = 1;     // Sát thương khi trúng thân
    constexpr int   SHOOT_COST = 1;           // Chi phí bắn (độ dài rắn bị giảm)

    // ==== THÔNG SỐ PIXEL (KHÔNG GRID) ====
    constexpr float WORLD_W_PX = GRID_W * CELL * 1.f;  // Chiều rộng thế giới (pixel)
    constexpr float WORLD_H_PX = GRID_H * CELL * 1.f;  // Chiều cao thế giới (pixel)

    constexpr float SNAKE_RADIUS_PX = CELL * 0.9f;       // Bán kính thân rắn (pixel)
    constexpr float SNAKE_SPEED_PX  = SNAKE_SPEED_CPS * CELL; // Tốc độ rắn (pixel/giây)
    constexpr float SNAKE_TURN_DEG_PER_S = 420.f;        // Tốc độ quay (độ/giây)

    constexpr float BULLET_RADIUS_PX = CELL * 0.5f;       // Bán kính đạn (pixel)
    constexpr float BULLET_SPEED_PX  = BULLET_SPEED_CPS * CELL; // Tốc độ đạn (pixel/giây)

    constexpr float ITEM_RADIUS_PX = CELL * 0.7f;         // Bán kính item (pixel)
}
