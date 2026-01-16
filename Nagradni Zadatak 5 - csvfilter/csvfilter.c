#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_COLS 256
#define MAX_COND 64

typedef enum { L, G, E, LE, GE, NE } OperationType;
typedef enum { NUMBER, STRING } ValueType;

typedef struct {
	OperationType operation;
	unsigned int column;
	ValueType type;
	double number;
	char* string;
} Condition;

int isNumber(const char* string, double* out) {
	char* end;
	double value = strtod(string, &end);
	if (*string == '\0' || end != '\0') return 0;
	if (out) *out = value;
	return 1;
}

OperationType parseOperation(const char input) {
	if (strcmp(input, "-L") == 0) return L;
	if (strcmp(input, "-G") == 0) return G;
	if (strcmp(input, "-E") == 0) return E;
	if (strcmp(input, "-LE") == 0) return LE;
	if (strcmp(input, "-GE") == 0) return GE;
	if (strcmp(input, "-NE") == 0) return NE;
	fprintf(stderr, "Uknown operation: %s\n", input);
	exit(1);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s *.csv [-Operation -column -value], Operation: L, G, E, LE, GE, NE\n", argv[0]);
		return -1;
	}

	const char* filename = argv[1];

	Condition conditions[MAX_COND];
	int numberOfConditions = 0;

	int i = 2;

	while (i + 2 < argc && numberOfConditions < MAX_COND) {
		OperationType operation = parseOperation(argv[i]);
		unsigned int column = atoi(argv[i + 1]);
		const char* value = argv[i + 2];

		conditions[numberOfConditions].operation = operation;
		conditions[numberOfConditions].column = column;

		double number;

		if (isNumber(value, &number)) {
			conditions[numberOfConditions].type = NUMBER;
			conditions[numberOfConditions].number = number;
			conditions[numberOfConditions].string = NULL;
		}
		else {
			conditions[numberOfConditions].type = STRING;
			conditions[numberOfConditions].string = strdup(value);
		}
		numberOfConditions++;
		i += 3;
	}

	FILE* filePointer = fopen(filename, "r");
	if (!filePointer) {
		perror("Can't open file.\n");
		return -1;
	}

	char line[MAX_LINE];

	while (fgets(line, sizeof(line), filePointer)) {
		char* original[MAX_LINE];
		strcpy(original, line);

		line[strcspn(line, "\r\n")] == '\0';

		char* columns[MAX_COLS];
		unsigned int numberOfColumns = 0;
		char* token = strtok(line, ",");
		while (token && numberOfColumns < MAX_COLS) {
			columns[numberOfColumns++] = token;
			token = strtok(NULL, ",");
		}

		int pass = 1;
		for (int j = 0; j < numberOfConditions; j++) {
			Condition* condition = &conditions[j];

			if (condition->column < 0 || condition->column >= numberOfColumns) {
				continue;
			}

			char* field = columns[condition->column];
			int ok = 0;

			if (condition->type == NUMBER) {
				double value;
				if (!isNumber(field, &value)) {
					pass = 0;
					break;
				}
				switch (condition->operation) {
					case L: ok = value < condition->number; break;
					case G: ok = value > condition->number; break;
					case E: ok = value == condition->number; break;
					case LE: ok = value <= condition->number; break;
					case GE: ok = value >= condition->number; break;
					case NE: ok = value != condition->number; break;
					default:
						break;
				}
			}
			else {
				int compare = strcmp(field, condition->string);

				switch (condition->operation) {
					case L: ok = compare < 0; break;
					case G: ok = compare > 0; break;
					case E: ok = compare == 0; break;
					case LE: ok = compare <= 0; break;
					case GE: ok = compare >= 0; break;
					case NE: ok = compare != 0; break;
					default:
						break;
				}
			}

			if (!ok) pass = 0;
		}

		if (pass) {
			fputs(original, stdout);
		}
	}

	fclose(filePointer);

	for (int j = 0; j < numberOfConditions; j++) {
		if (conditions[j].type == STRING) {
			free(conditions[j].string);
		}
	}

	return 0;
}