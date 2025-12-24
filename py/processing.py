signals_10s = []

with open("data_24-12-25-14-00.txt", "r") as file:
    for line in file:
        if line != "\n":
            signals_10s.append(int(line[:-1]))

print(signals_10s)

with open("data_24-12-25-14-00.csv", "w") as file:
    for x in signals_10s:
        file.write(str(x)+";")

import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import gaussian_kde

print(f"Всего измерений: {len(signals_10s)}")
print(f"Длительность наблюдений: {len(signals_10s) * 10} секунд")

# Создание данных для 30-секундных интервалов (сумма 3 последовательных измерений)
signals_30s = []
for i in range(0, len(signals_10s) - 2, 3):
    signals_30s.append(signals_10s[i] + signals_10s[i+1] + signals_10s[i+2])

# Создание данных для 60-секундных интервалов (сумма 6 последовательных измерений)
signals_60s = []
for i in range(0, len(signals_10s) - 5, 6):
    signals_60s.append(signals_10s[i] + signals_10s[i+1] + signals_10s[i+2] + 
                      signals_10s[i+3] + signals_10s[i+4] + signals_10s[i+5])

print(f"\nИнтервал 10 сек: {len(signals_10s)} измерений")
print(f"Интервал 30 сек: {len(signals_30s)} измерений")
print(f"Интервал 60 сек: {len(signals_60s)} измерений")

# Создание фигуры с тремя графиками
fig, axes = plt.subplots(1, 3, figsize=(15, 5))
fig.suptitle('Плотность распределения количества сигналов для разных интервалов', 
             fontsize=14, fontweight='bold', y=1.02)

# Настройка цветовой палитры
colors = ['#1f77b4', '#ff7f0e', '#2ca02c']  # Синий, оранжевый, зеленый
interval_labels = ['10 секунд', '30 секунд', '60 секунд']
datasets = [signals_10s, signals_30s, signals_60s]

for idx, (ax, data, color, label) in enumerate(zip(axes, datasets, colors, interval_labels)):
    
    # Вычисляем базовую статистику
    mean_val = np.mean(data)
    median_val = np.median(data)
    std_val = np.std(data)
    
    # Определяем диапазон для графика
    x_min = min(data) - 0.1 * (max(data) - min(data))
    x_max = max(data) + 0.1 * (max(data) - min(data))
    x_smooth = np.linspace(x_min, x_max, 1000)
    
    # 1. Гистограмма с нормировкой по плотности
    # Оптимальное количество бинов по правилу Фридмана-Дайкона
    data_range = max(data) - min(data)
    bin_width = 2 * (np.percentile(data, 75) - np.percentile(data, 25)) / (len(data) ** (1/3))
    n_bins = max(5, int(np.ceil(data_range / bin_width)))  # минимум 5 бинов
    
    ax.hist(data, bins=n_bins, density=True, alpha=0.5, 
            color=color, edgecolor='white', linewidth=1.2, 
            label=f'Гистограмма ({n_bins} интервалов)')
    
    # 2. KDE (Ядерная оценка плотности)
    kde = gaussian_kde(data)
    ax.plot(x_smooth, kde(x_smooth), 'k-', linewidth=2.5, 
            label='KDE оценка плотности')
    
    # 3. Вертикальные линии для статистик
    ax.axvline(mean_val, color='red', linestyle='--', linewidth=1.5, 
               alpha=0.8, label=f'Среднее = {mean_val:.1f}')
    ax.axvline(median_val, color='purple', linestyle=':', linewidth=1.5, 
               alpha=0.8, label=f'Медиана = {median_val:.1f}')
    
    # Настройка графика
    ax.set_xlabel(f'Количество сигналов за {label}', fontsize=11)
    ax.set_ylabel('Плотность вероятности', fontsize=11)
    ax.set_title(f'Интервал: {label}\n(n={len(data)})', fontsize=12, fontweight='bold')
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.legend(loc='upper right', fontsize=9)
    
    # Добавляем текстовую информацию о статистике
    stats_text = (f'Среднее: {mean_val:.1f}\n'
                  f'Медиана: {median_val:.1f}\n'
                  f'Стд. откл.: {std_val:.1f}\n'
                  f'Минимум: {min(data)}\n'
                  f'Максимум: {max(data)}')
    
    # Размещаем текст в зависимости от номера графика
    if idx == 0:  # Левый график
        ax.text(0.02, 0.98, stats_text, transform=ax.transAxes,
                fontsize=9, verticalalignment='top',
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
    else:  # Центральный и правый графики
        ax.text(0.98, 0.98, stats_text, transform=ax.transAxes,
                fontsize=9, verticalalignment='top', horizontalalignment='right',
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

# Автоматическая настройка пределов по оси Y для лучшей визуализации
y_limits = [ax.get_ylim()[1] for ax in axes]
max_y_limit = max(y_limits) * 1.1
for ax in axes:
    ax.set_ylim(0, max_y_limit)

plt.tight_layout()

plt.savefig('plot_1_east.jpg')

# Дополнительно: один большой график с наложением всех трех распределений
plt.figure(figsize=(10, 6))

for data, color, label in zip(datasets, colors, interval_labels):
    # Создаем KDE для каждого набора данных
    kde = gaussian_kde(data)
    
    # Нормализуем данные для лучшего сравнения форм распределений
    normalized_data = (data - np.mean(data)) / np.std(data)
    kde_norm = gaussian_kde(normalized_data)
    x_norm = np.linspace(-4, 4, 1000)
    
    plt.plot(x_norm, kde_norm(x_norm), color=color, linewidth=2.5, 
             label=f'{label} (n={len(data)})')

plt.xlabel('Нормализованное количество сигналов (в единицах σ)', fontsize=12)
plt.ylabel('Плотность вероятности', fontsize=12)
plt.title('Сравнение форм распределений (нормализованные данные)', 
          fontsize=13, fontweight='bold')
plt.grid(True, alpha=0.3, linestyle='--')
plt.legend(loc='upper right')

plt.tight_layout()
plt.savefig('plot_2_east.jpg')

# Вывод информации о данных
print("\n" + "="*50)
print("СВОДНАЯ СТАТИСТИКА:")
print("="*50)
print(f"{'Интервал':<10} {'N':<6} {'Среднее':<10} {'Медиана':<10} {'Стд.откл.':<10} {'Min':<8} {'Max':<8}")
print("-"*70)

for label, data in zip(interval_labels, datasets):
    print(f"{label:<10} {len(data):<6} {np.mean(data):<10.1f} {np.median(data):<10.1f} "
          f"{np.std(data):<10.1f} {min(data):<8} {max(data):<8}")