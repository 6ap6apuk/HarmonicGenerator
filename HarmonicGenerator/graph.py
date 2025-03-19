import matplotlib.pyplot as plt
import numpy as np

# Чтение данных из файла
def read_data_from_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
    
    # Извлекаем заголовки
    headers = lines[0].strip().split('\t')
    data = []
    
    # Извлекаем данные
    for line in lines[1:]:
        values = list(map(float, line.strip().split('\t')))
        data.append(values)
    
    return headers, np.array(data)

# Функция для построения графиков
def plot_harmonics(headers, data):
    time_column = headers.index('Время')
    harmonics_columns = [header for header in headers if 'Гармоника_' in header]
    resultant_column = headers[-1]
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Подключаем циклический набор цветов
    cmap = plt.get_cmap('tab10')  # Можно выбрать любой другой cmap, например, 'rainbow' или 'viridis'
    
    # Строим графики гармоник
    for i, column_name in enumerate(harmonics_columns):
        index = headers.index(column_name)
        ax.plot(data[:, time_column], data[:, index], label=column_name, color=cmap(i))  # Используем cmap для назначения цвета
    
    # Строим график результирующей гармоники
    index = headers.index(resultant_column)
    ax.plot(data[:, time_column], data[:, index], label=resultant_column, linewidth=3, color='k')
    
    # Настройки графика
    ax.set_xlabel('Время')
    ax.set_ylabel('Значение')
    ax.legend(loc='best')
    ax.grid(True)
    
    plt.show()

# Основная функция
if __name__ == "__main__":
    filename = 'result.txt'
    headers, data = read_data_from_file(filename)
    plot_harmonics(headers, data)