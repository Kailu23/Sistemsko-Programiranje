#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096
#define MAX_COLS 256

int isNumber(const char* string, double* out) {
	char* end;
	double value = strtod(string, &end);
	if (*string == '\0' || *end != '\0') return 0;
	if (out) *out = value;
	return 1;
}

int SplitCsvLine(char* line, char* columns[], int maxColumns) {
	int numberOfColumns = 0;
	char* token = strtok(line, ",");
	while (token && numberOfColumns < maxColumns) {
		columns[numberOfColumns++] = token;
		token = strtok(NULL, ",");
	}
	return numberOfColumns;
}

int readInputLines(char*** outLines) {
	char line[MAX_LINE];
	char** lines = NULL;
	unsigned int numberOfLines = 0;
	unsigned int linesCapacity = 0;

	while (fgets(line, sizeof(line), stdin)) {
		if (linesCapacity == numberOfLines) {
			linesCapacity = linesCapacity ? linesCapacity * 2 : 16;
			lines = (char**)realloc(lines, linesCapacity * sizeof(char*));
			if (!lines) {
				fprintf(stderr, "Memory error at realloc lines\n");
				return -3;
			}
		}
		lines[numberOfLines] = _strdup(line);
		if (!lines[numberOfLines]) {
			fprintf(stderr, "Memory error at _strdup line\n");
			return -3;
		}
		numberOfLines++;
	}
	*outLines = lines;
	return numberOfLines;
}

int getColumnCountAndAggregateRows(char** lines, int totalLines, int* outDataLines, char** outAverageLine, char** outMinLine, char** outMaxLine) {
	if (totalLines < 3) return 0;

	int dataLines = totalLines - 3;
	*outDataLines = dataLines;
	*outAverageLine = lines[totalLines - 3];
	*outMinLine = lines[totalLines - 2];
	*outMaxLine = lines[totalLines - 1];

	unsigned int numberOfColumns = 0;
	char tmp[MAX_LINE];
	char* columns[MAX_COLS];

	if (dataLines > 0) {
		strcpy(tmp, lines[0]);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		numberOfColumns = SplitCsvLine(tmp, columns, MAX_COLS);
	}
	else {
		strcpy(tmp, *outAverageLine);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		numberOfColumns = SplitCsvLine(tmp, columns, MAX_COLS);
	}

	return numberOfColumns;
}

void SplitAggregateRows(char* averageLine, char* minLine, char* maxLine, char* averageColumns[], char* minColumns[], char* maxColumns[]) {

	averageLine[strcspn(averageLine, "\r\n")] = '\0';
	minLine[strcspn(minLine, "\r\n")] = '\0';
	maxLine[strcspn(maxLine, "\r\n")] = '\0';

	SplitCsvLine(averageLine, averageColumns, MAX_COLS);
	SplitCsvLine(minLine, minColumns, MAX_COLS);
	SplitCsvLine(maxLine, maxColumns, MAX_COLS);
}

void PrintJSONData(char** lines, int dataLines, int totalColumns) {
	char tmp[MAX_LINE];
	char* dataColumns[MAX_COLS];

	printf("\t\"Data\": [\n");

	for (int i = 0; i < dataLines; i++) {
		strcpy(tmp, lines[i]);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		int totalDataColumns = SplitCsvLine(tmp, dataColumns, MAX_COLS);

		printf("\t\t{");
		for (int c = 0; c < totalColumns && c < totalDataColumns; c++) {
			char* string = dataColumns[c];
			double number;
			if (isNumber(string, &number)) {
				printf("\"Column%d\": %.4lf", c + 1, number);
			}
			else {
				printf("\"Column%d\": \"%s\"", c + 1, string);
			}

			if (c < totalColumns - 1) printf(", ");
		}
		printf("}");
		if (i < dataLines - 1) printf(", ");
		printf("\n");
	}
	printf("\t],\n");
}

void PrintJSONAggregate(int totalColumns, char* averageColumns[], char* minColumns[], char* maxColumns[]) {
	char* averageValue;
	char* minValue;
	char* maxValue;

	printf("\t\"Aggregates\": {\n");

	printf("\t\t\"average_or_most_frequent\": \n\t\t\t{");
	for (int c = 0; c < totalColumns; c++) {
		averageValue = averageColumns[c] ? averageColumns[c] : "";

		double number;
		if (isNumber(averageValue, &number)) {
			printf("\"Column%d\": %lf", c + 1, number);
		}
		else {
			printf("\"Column%d\": \"%s\"", c + 1, averageValue);
		}
		if (c < totalColumns - 1) printf(", ");
	}
	printf("},\n");

	printf("\t\t\"minimum\": \n\t\t\t{");
	for (int c = 0; c < totalColumns; c++) {
		minValue = minColumns[c] ? minColumns[c] : "";

		double number;
		if (isNumber(minValue, &number)) {
			printf("\"Column%d\": %lf", c + 1, number);
		}
		else {
			printf("\"Column%d\": \"%s\"", c + 1, minValue);
		}
		if (c < totalColumns - 1) printf(", ");
	}
	printf("},\n");

	printf("\t\t\"maximum\": \n\t\t\t{");
	for (int c = 0; c < totalColumns; c++) {
		maxValue = maxColumns[c] ? maxColumns[c] : "";

		double number;
		if (isNumber(maxValue, &number)) {
			printf("\"Column%d\": %lf", c + 1, number);
		}
		else {
			printf("\"Column%d\": \"%s\"", c + 1, maxValue);
		}
		if (c < totalColumns - 1) printf(", ");
	}
	printf("}\n");

	printf("\t}\n");
}


int main(void) {
	char** lines = NULL;
	unsigned int totalLines = readInputLines(&lines);
	if (totalLines < 0) {;
		return 1;
	}

	if (totalLines < 3) {
		for (unsigned int i = 0; i < totalLines; i++) free(lines[i]);
		free(lines);
		return -3;
	}

	int dataLines;
	char* averageLine;
	char* minLine;
	char* maxLine;

	int totalColumns = getColumnCountAndAggregateRows(lines, totalLines, &dataLines, &averageLine, &minLine, &maxLine);

	if (totalColumns <= 0) {
		for (unsigned int i = 0; i < totalLines; i++) free(lines[i]);
		free(lines);
		return 0;
	}

	char* averageColumns[MAX_COLS];
	char* minColumns[MAX_COLS];
	char* maxColumns[MAX_COLS];

	SplitAggregateRows(averageLine, minLine, maxLine, averageColumns, minColumns, maxColumns);
	printf("{\n");
	PrintJSONData(lines, dataLines, totalColumns);
	PrintJSONAggregate(totalColumns, averageColumns, minColumns, maxColumns);
	printf("}\n");

	for (unsigned int i = 0; i < totalLines; i++) free(lines[i]);
	free(lines);
	return 0;
}