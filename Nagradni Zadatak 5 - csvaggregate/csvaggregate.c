#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096
#define MAX_COLS 256

typedef struct _NumberAggregate {
	int isNumeric;
	double sum;
	double min;
	double max;
	long count;
} NumberAggregate;

typedef struct _StringFrequency {
	char* value;
	long frequency;
} StringFrequency;

typedef struct _StringAggregate {
	int isNumeric;
	StringFrequency* items;
	int size;
	int cap;
	char* min;
	char* max;
} StringAggregate;

int SplitCsvLine(char* line, char* columns[], int maxColumns) {
	int numberOfColumns = 0;
	char* token = strtok(line, ",");
	while (token && numberOfColumns < maxColumns) {
		columns[numberOfColumns++] = token;
		token = strtok(NULL, ",");
	}
	return numberOfColumns;
}

int isNumber(const char* string, double* out) {
	char* end;
	double value = strtod(string, &end);
	if (*string == '\0' || *end != '\0') return 0;
	if (out) *out = value;
	return 1;
}

StringFrequency* stringFindOrAdd(StringAggregate* aggregate, const char* string) {
	for (int i = 0; i < aggregate->size; ++i) {
		if (strcmp(aggregate->items[i].value, string) == 0) {
			return &aggregate->items[i];
		}
	}

	if (aggregate->size == aggregate->cap) {
		aggregate->cap = aggregate->cap ? aggregate->cap * 2 : 8;
		aggregate->items = (StringFrequency*)realloc(aggregate->items, aggregate->cap * sizeof(StringFrequency));
		if (!aggregate->items) {
			fprintf(stderr, "Memory error in stringFindOrAdd\n");
			exit(1);
		}
	}
	aggregate->items[aggregate->size].value = _strdup(string);
	aggregate->items[aggregate->size].frequency = 0;
	aggregate->size++;
	return &aggregate->items[aggregate->size - 1];
}

void stringAdd(StringAggregate* aggregate, const char* string) {
	StringFrequency* stringFrequency = stringFindOrAdd(aggregate, string);
	stringFrequency->frequency++;

	if (!aggregate->min || strcmp(string, aggregate->min) < 0) {
		free(aggregate->min);
		aggregate->min = _strdup(string);
	}
	if (!aggregate->max || strcmp(string, aggregate->max) > 0) {
		free(aggregate->max);
		aggregate->max = _strdup(string);
	}
}

void processAllLines(char** lines, int numberOfLines, int numberOfColumns, int isNumericColumn[], NumberAggregate numberAggregate[], StringAggregate stringAggregate[]) {
	char tmp[MAX_LINE];

	for (int i = 0; i < numberOfLines; ++i) {
		fputs(lines[i], stdout);

		strcpy(tmp, lines[i]);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		char* fileColumns[MAX_COLS];
		int columnCount = SplitCsvLine(tmp, fileColumns, MAX_COLS);

		for (int c = 0; c < numberOfColumns && c < columnCount; ++c) {
			if (isNumericColumn[c]) {
				double value;
				if (!isNumber(fileColumns[c], &value)) {
					continue;
				}
				if (numberAggregate[c].count == 0) {
					numberAggregate[c].min = value;
					numberAggregate[c].max = value;
				}
				numberAggregate[c].sum += value;
				if (value < numberAggregate[c].min) numberAggregate[c].min = value;
				if (value > numberAggregate[c].max) numberAggregate[c].max = value;
				numberAggregate[c].count++;
			}
			else {
				stringAdd(&stringAggregate[c], fileColumns[c]);
			}
		}
	}
}
void printAverageOrModeLine(int numberOfColumns, int isNumericColumn[], NumberAggregate numberAggregate[], StringAggregate stringAggregate[]) {
	for (int c = 0; c < numberOfColumns; ++c) {
		if (isNumericColumn[c]) {
			double average = numberAggregate[c].count ? numberAggregate[c].sum / numberAggregate[c].count : 0.0;
			printf("%.6f", average);
		}
		else {
			if (stringAggregate[c].size == 0) {
				printf("");
			}
			else {
				StringFrequency* best = &stringAggregate[c].items[0];
				for (int k = 1; k < stringAggregate[c].size; ++k) {
					if (stringAggregate[c].items[k].frequency > best->frequency) {
						best = &stringAggregate[c].items[k];
					}
				}
					printf("%s(%ld)", best->value, best->frequency);
			}
		}
		if (c < numberOfColumns - 1) printf(",");
	}
	printf("\n");
}

void printMinLine(int numberOfColumns, int isNumericColumn[], NumberAggregate numberAggregate[], StringAggregate stringAggregate[]) {
	for (int c = 0; c < numberOfColumns; ++c) {
		if (isNumericColumn[c]) {
			printf("%.4lf", numberAggregate[c].min);
		}
		else {
			if (stringAggregate[c].min) printf("%s", stringAggregate[c].min);
		}
		if (c < numberOfColumns - 1) printf(",");
	}
	printf("\n");
}

void printMaxLine(int numberOfColumns, int isNumericColumn[], NumberAggregate numberAggregate[], StringAggregate stringAggregate[]) {
	for (int c = 0; c < numberOfColumns; ++c) {
		if (isNumericColumn[c]) {
			printf("%.4lf", numberAggregate[c].max);
		}
		else {
			if (stringAggregate[c].max) printf("%s", stringAggregate[c].max);
		}
		if (c < numberOfColumns - 1) printf(",");
	}
	printf("\n");
}
int main(void) {
	char line[MAX_LINE];

	char** lines = NULL;
	unsigned int numberOfLines = 0;
	unsigned int linesCapacity = 0;

	while (fgets(line, sizeof(line), stdin)) {
		if (linesCapacity == numberOfLines) {
			linesCapacity = linesCapacity ? linesCapacity * 2 : 16;
			lines = (char**)realloc(lines, linesCapacity * sizeof(char*));

			if (!lines) {
				fprintf(stderr, "Memory error at realloc\n");
				return -2;
			}
		}

		lines[numberOfLines] = _strdup(line);
		if (!lines[numberOfLines]) {
			fprintf(stderr, "Memory error at strdup line\n");
			return -2;
		}
		numberOfLines++;
	}

	if (numberOfLines == 0) {
		return 0;
	}

	char tmp[MAX_LINE];
	strcpy(tmp, lines[0]);
	tmp[strcspn(tmp, "\r\n")] = '\0';
	char* columns[MAX_COLS];
	int numberOfColumns = SplitCsvLine(tmp, columns, MAX_COLS);

	int isNumericColumn[MAX_COLS] = { 0 };
	NumberAggregate numberAggregate[MAX_COLS];
	StringAggregate stringAggregate[MAX_COLS];

	for (int c = 0; c < numberOfColumns; ++c) {
		double value;
		if (isNumber(columns[c], &value)) {
			isNumericColumn[c] = 1;
			numberAggregate[c].isNumeric = 1;
			numberAggregate[c].sum = 0.0;
			numberAggregate[c].min = value;
			numberAggregate[c].max = value;
			numberAggregate[c].count = 0;
		}
		else {
			isNumericColumn[c] = 0;
			stringAggregate[c].isNumeric = 0;
			stringAggregate[c].items = NULL;
			stringAggregate[c].size = 0;
			stringAggregate[c].cap = 0;
			stringAggregate[c].min = NULL;
			stringAggregate[c].max = NULL;
		}
	}

	processAllLines(lines, numberOfLines, numberOfColumns, isNumericColumn, numberAggregate, stringAggregate);
	printAverageOrModeLine(numberOfColumns, isNumericColumn, numberAggregate, stringAggregate);
	printMinLine(numberOfColumns, isNumericColumn, numberAggregate, stringAggregate);
	printMaxLine(numberOfColumns, isNumericColumn, numberAggregate, stringAggregate);

	for (int c = 0; c < numberOfColumns; ++c) {
		if (!isNumericColumn[c]) {
			for (int k = 0; k < stringAggregate[c].size; ++k) {
				free(stringAggregate[c].items[k].value);
			}
			free(stringAggregate[c].items);
			free(stringAggregate[c].min);
			free(stringAggregate[c].max);
		}
	}

	for (int l = 0; l < numberOfLines; ++l) {
		free(lines[l]);
	}
	free(lines);
	
	return 0;
}