#pragma once
#include <type_traits>
#include "ClassTemplateVector.h"

template <typename T>
class Matrix : public Vector<Vector<T>> {
public:
	Matrix(size_t column, const Vector<T>& sample): Vector<Vector<T>>(column, sample) {}
};

template <typename T>
class GaussianMethod : public Matrix<T> {
protected:
	T zero; // ���� ���� � (��������� ��� ��������� � ��� ���������)

	size_t SearchMaxInColumn(const GaussianMethod<T>& system, size_t column_size, size_t column) {
		// ����� ������������� �������� � �������
		T element = system[column][column]; // ������������ �� ������ �������
		size_t max_point = column; // ������ ������������� �� ������ �������� � �������
		size_t count = 0; // ������� ��������� ��������� ������� (���� column)

		for (size_t i = column; i < column_size; i++) {
			if (system[i][column] != zero) count++;
			if (system[i][column] * system[i][column] > element * element) {
				element = system[i][column];
				max_point = i;
			}
		}

		if (count == 0) max_point = -1;
		return max_point;
	}

	void MatrixZeroTrimmer(GaussianMethod<T>& system, size_t m, size_t n) { // ��������� ����� �� ������ ��������� ������������ ����� (� �������)
		double eps = 10e-7; // ������� ������� ����������� ��� ��������� � 0
		if (std::is_same<T, float>::value || std::is_same<T, double>::value)
			for (size_t i = 0; i < m; i++)
				for (size_t j = 0; j < n + 1; j++)
					if (abs(system[i][j]) < eps) system[i][j] = zero;
	}

	void VectorZeroTrimmer(Vector<T>& vector, size_t n) { // ��������� ����� �� ������ ��������� ������������ ����� (� �������-������)
		double eps = 10e-8; // ������� ������� ����������� ��� ��������� � 0
		if (std::is_same<T, float>::value || std::is_same<T, double>::value)
			for (size_t i = 0; i < n; i++)
				if (abs(vector[i]) < eps) vector[i] = zero;
	}

	void CheckSystem(GaussianMethod<T>& system, size_t m, size_t n, size_t current_column) {
		// �������� ������� �� ������������� (��� �������) � ������� ��������� ���������� (���������� ����� �������)
		size_t non_zero_row = 0; // ������� ��������� �����
		for (size_t i = 0; i < m; i++)
		{
			size_t count = 0; // ������� ��������� ��������� ������ (��� ���������� ������������)
			for (size_t j = 0; j < n + 1; j++) {
				if (system[i][j] != zero && j != n) count++;
			}
			if (count == 0 && system[i][n] != zero) error(2); // ������� ����������� (������ �������, � ��������� ����������� - ���)
			else if (!(count == 0 && system[i][n] == zero)) non_zero_row++; // ������� ��������� ����� � ������������� �������
		}
		if (non_zero_row < current_column + 1) error(3);
		// ������� ���������� �����, �.�. �������� ������, ��� ��������� ����� => ���� ��������� �����������;
	}

	bool ZeroColumnCheck(const GaussianMethod<T>& system, size_t row, size_t column) {
		// �������� �� ������� ������� �������� (��� ������� ������� ������)
		for (size_t i = 0; i < column; i++) {
			int count = 0;
			for (size_t j = 0; j < row; j++)
				if (system[j][i] != zero) count++;
			if (count == 0) return true;
		}
		return false;
	}

public:
	GaussianMethod(size_t column, const Vector<T>& sample, const T& zero) : Matrix<T>(column, sample) {
		this->zero = zero;
	}

	Vector<T> Solve() {

		size_t column_size = this->GetSize(); // ���������� ����� � �������
		size_t row_size = ((*this)[0].GetSize()) - 1; // ���������� �������� � ������� (��� ����� ������� ��������� �������������)
		Vector<T> solution(row_size, zero); // ������ �������
		size_t max_point; // ������� �������� �������� � �������

		if (ZeroColumnCheck(*this, column_size, row_size)) error(4);
		else {
			for (size_t i = 0; i < row_size; i++) {
				CheckSystem(*this, column_size, row_size, i); // �������� ����
				max_point = SearchMaxInColumn(*this, column_size, i);
				if (max_point != -1) {
					std::swap<Vector<T>>((*this)[i], (*this)[max_point]); // ������ ������� ������� �� ������� ���������
					(*this)[i] = (*this)[i]/(*this)[i][i];
					for (size_t j = i + 1; j < column_size; j++) // �������� �� ���� �������� ��������
						(*this)[j] -= ((*this)[i] * (*this)[j][i]);
					MatrixZeroTrimmer(*this, column_size, row_size); // �������� ������� � ���� �������� ������������ ����� � �������
					cout << "\t" << *this;
				}
				else error(3);
			}
		}

		// ���������� �������-�������, ���� ������� ������������
		for (int i = row_size - 1; i >= 0; i--) {
			solution[i] = (*this)[i][row_size];
			for (int j = row_size - 1; j >= i + 1; j--)
				solution[i] -= (*this)[i][j] * solution[j];
		}

		VectorZeroTrimmer(solution, row_size); // �������� ������� � ���� �������� ������������ ����� � �������
		return solution;

	}

};