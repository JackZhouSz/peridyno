#include "Turning.h"
#include "Topology/PointSet.h"
#include "GLSurfaceVisualModule.h"
#include "GLWireframeVisualModule.h"

namespace dyno
{
	template<typename TDataType>
	TurningModel<TDataType>::TurningModel()
		: ParametricModel<TDataType>()
	{
		this->varColumns()->setRange(3, 50);
		this->varRadius()->setRange(-10.0f, 10.0f);
		this->varEndSegment()->setRange(2, 39);

		this->stateTriangleSet()->setDataPtr(std::make_shared<TriangleSet<TDataType>>());

		auto glModule = std::make_shared<GLSurfaceVisualModule>();
		glModule->setColor(Color(0.8f, 0.52f, 0.25f));
		glModule->setVisible(true);
		this->stateTriangleSet()->connect(glModule->inTriangleSet());
		this->graphicsPipeline()->pushModule(glModule);

		this->inPointSet()->tagOptional(true);

		this->stateTriangleSet()->promoteOuput();

		auto callback = std::make_shared<FCallBackFunc>(std::bind(&TurningModel<TDataType>::varChanged, this));

		this->varLocation()->attach(callback);
		this->varScale()->attach(callback);
		this->varRotation()->attach(callback);

		this->varColumns()->attach(callback);
		this->varEndSegment()->attach(callback);
		this->varRadius()->attach(callback);
		this->varReverseNormal()->attach(callback);
		this->varUseRamp()->attach(callback);
		this->varCurve()->attach(callback);


	}

	template<typename TDataType>
	void TurningModel<TDataType>::resetStates()
	{
		this->varChanged();
	}


	template<typename TDataType>
	void TurningModel<TDataType>::varChanged()
	{
		printf("Turning Reset\n");
		auto center = this->varLocation()->getData();
		auto rot = this->varRotation()->getData();
		auto scale = this->varScale()->getData();

		auto columns = this->varColumns()->getData();
		auto end_segment = this->varEndSegment()->getData();
		int pointsize = 0;;
		if (!this->inPointSet()->isEmpty())
		{
			pointsize = this->inPointSet()->getData().getPointSize();
		}

		auto useRamp = this->varUseRamp()->getValue();
		auto Ramp = this->varCurve()->getValue();
		auto floatCoordArray = Ramp.FinalCoord;

		auto triangleSet = this->stateTriangleSet()->getDataPtr();

		Real PI = 3.1415926535;

		std::vector<Coord> vertices;
		std::vector<TopologyModule::Triangle> triangle;


		PointSet<TDataType> s;
		if (!this->inPointSet()->isEmpty())
		{
			s.copyFrom(this->inPointSet()->getData());
		}
		DArray<Coord> sa = s.getPoints();
		CArray<Coord> c_sa;
		PointSet<TDataType> pointSet;

		if (useRamp)
		{
			std::vector<Coord> vertices;
			std::vector<TopologyModule::Triangle> triangle;
			pointsize = floatCoordArray.size();

			Coord Location;
			if (pointsize != 0)
			{
				for (size_t i = 0; i < pointsize; i++)
				{
					Location = Coord(floatCoordArray[i].x, floatCoordArray[i].y, 0);

					vertices.push_back(Location);
				}
			}
			pointSet.setPoints(vertices);
			c_sa.assign(pointSet.getPoints());
		}
		else
		{
			c_sa.assign(sa);
		}
		std::cout << "Point number�� " << pointsize << std::endl;


		int row_i = pointsize;
		int columns_i = int(columns);
		uint counter = 0;
		Coord Location;
		Real angle = PI / 180 * 360 / columns_i;
		Real temp_angle = angle;

		//�����ǲ����Ĺ���
		if (!this->inPointSet()->isEmpty() | (useRamp && pointsize > 0))
		{
			for (int k = 0; k < row_i; k++)
			{
				Real tempy = c_sa[k][1];
				Real radius = c_sa[k][0] + this->varRadius()->getData();

				for (int j = 0; j < columns_i; j++) {

					temp_angle = j * angle;

					Location = { sin(temp_angle) * radius , tempy ,cos(temp_angle) * radius };

					vertices.push_back(Location);
				}
			}

			//�����ǵײ����ϲ���Ĺ���

			int pt_side_len = vertices.size();

			for (int i = 1; i < end_segment; i++)
			{
				float offset = i / (float(end_segment) - i);

				for (int p = 0; p < columns; p++)
				{
					Coord buttompt = { (vertices[p][0] + offset * 0) / (1 + offset), c_sa[0][1], (vertices[p][2] + offset * 0) / (1 + offset) };

					vertices.push_back(buttompt);
				}

			}

			for (int i = 1; i < end_segment; i++)
			{
				float offset = i / (float(end_segment) - i);

				for (int p = 0; p < columns; p++)
				{
					int top_start = pt_side_len - columns + p;

					Coord toppt = { (vertices[top_start][0] + offset * 0) / (1 + offset),  c_sa[c_sa.size() - 1][1], (vertices[top_start][2] + offset * 0) / (1 + offset) };

					vertices.push_back(toppt);
				}

			}


			//�����ǵײ�Բ�ļ��ϲ�Բ�ĵ�Ĺ���
			Real buttom = c_sa[0][1];
			Real height = c_sa[c_sa.size() - 1][1];
			vertices.push_back(Coord(0, buttom, 0));
			vertices.push_back(Coord(0, height, 0));

			//�����ǲ���Ĺ���
			for (int rowl = 0; rowl < row_i - 1; rowl++)
			{
				for (int faceid = 0; faceid < columns_i; faceid++)
				{

					if (faceid != columns_i - 1)
					{

						triangle.push_back(TopologyModule::Triangle(columns_i + faceid + rowl * columns_i, 0 + faceid + rowl * columns_i, 1 + faceid + rowl * columns_i));
						triangle.push_back(TopologyModule::Triangle(columns_i + 1 + faceid + rowl * columns_i, columns_i + faceid + rowl * columns_i, 1 + faceid + rowl * columns_i));
					}
					else
					{
						triangle.push_back(TopologyModule::Triangle(1 + 2 * faceid + rowl * columns_i, 0 + faceid + rowl * columns_i, 0 + rowl * columns_i));
						triangle.push_back(TopologyModule::Triangle(1 + faceid + rowl * columns_i, 1 + 2 * faceid + rowl * columns_i, 0 + rowl * columns_i));
					}

				}
			}

			//�����ǵ���Ͷ���Ĺ���

			//�����ǵ���Ͷ���Ĺ���
			//����ԭ�еĵ�����pt_side_len,

			int pt_len = vertices.size() - 2;
			int top_pt_len = vertices.size() - 2 - pt_side_len;
			int addnum = 0;

			for (int s = 0; s < end_segment; s++)
			{


			}
			for (int s = 0; s < end_segment; s++)  //�ڲ�ѭ������ÿһȦÿһ��
			{
				int temp = 0;
				//****************�Ƿ�����Ȧ������Ȧʹ���ĸ���Χ������������*****************//
				if (s != end_segment - 1)
				{
					for (int i = 0; i < columns; i++)
					{
						//****************���ж��Ƿ�������һȦ���ǵĻ������������*****************//
						if (s == 0)
						{
							temp = i;  //iΪ0-columns����ţ���+ x * (pt_side_len - columns)����Ϊ������ŵı仯�������յó����� �ϡ���һȦ�����
							addnum = pt_side_len;
						}
						else
						{
							temp = pt_side_len + i + unsigned(s - 1) * columns;
							addnum = columns;
						}


						//****************�Ƿ������һ�У��ǵĻ���β�����ӣ���ֹ���ӵ㻻��*****************//
						if (i != columns - 1)
						{
							triangle.push_back(TopologyModule::Triangle(addnum + temp, temp + 1, temp));	//���ɵ���
							triangle.push_back(TopologyModule::Triangle(addnum + temp, addnum + temp + 1, temp + 1));
						}
						else
						{
							triangle.push_back(TopologyModule::Triangle(addnum + temp, temp - columns + 1, temp));	//���ɵ������һ��

							if (s == 0)		triangle.push_back(TopologyModule::Triangle(addnum + temp, temp - columns + addnum + 1, temp - columns + 1));
							else			triangle.push_back(TopologyModule::Triangle(addnum + temp, temp + 1, temp - columns + 1));

						}

					}
				}
				//****************�Ƿ�������Ȧ��������Ȧʹ���ܳ�����Բ��*****************//
				else
				{

					for (int z = 0; z < columns; z++)
					{
						temp = pt_side_len + z + unsigned(s - 1) * columns;
						if (z != columns - 1)
						{
							triangle.push_back(TopologyModule::Triangle(temp + 1, temp, pt_len));	//���ɵ�������Ȧ

						}
						else
						{
							triangle.push_back(TopologyModule::Triangle(temp - columns + 1, temp, pt_len));	//���ɵ�������Ȧ���һ����

						}

					}
				}

			}
			//*************************�ϲ�************************//

			for (int s = 0; s < end_segment; s++)  //�ڲ�ѭ������ÿһȦÿһ��
			{
				int temp = 0;
				//****************�Ƿ�����Ȧ������Ȧʹ���ĸ���Χ������������*****************//
				if (s != end_segment - 1)
				{
					for (int i = 0; i < columns; i++)
					{
						//****************���ж��Ƿ�������һȦ���ǵĻ������������*****************//
						if (s == 0)
						{
							temp = i + pt_side_len - columns;  //iΪ0-columns����ţ���+ x * (pt_side_len - columns)����Ϊ������ŵı仯�������յó����� �ϡ���һȦ�����
							addnum = end_segment * columns;;
						}
						else
						{
							temp = pt_side_len + i + unsigned(s - 1) * columns + columns * (end_segment - 1);
							addnum = columns;
						}
						//****************�Ƿ������һ�У��ǵĻ���β�����ӣ���ֹ���ӵ㻻��*****************//
						if (i != columns - 1)
						{
							triangle.push_back(TopologyModule::Triangle(temp, temp + 1, addnum + temp));	//���ɵ���
							triangle.push_back(TopologyModule::Triangle(temp + 1, addnum + temp + 1, addnum + temp));
						}
						else
						{
							triangle.push_back(TopologyModule::Triangle(temp, temp - columns + 1, addnum + temp));	//���ɵ������һ��

							if (s == 0)		triangle.push_back(TopologyModule::Triangle(temp - columns + 1, temp - columns + addnum + 1, addnum + temp));
							else			triangle.push_back(TopologyModule::Triangle(temp - columns + 1, temp + 1, addnum + temp));

						}

					}
				}
				//****************�Ƿ�������Ȧ��������Ȧʹ���ܳ�����Բ��*****************//
				else
				{

					for (int z = 0; z < columns; z++)
					{
						temp = pt_side_len + z + unsigned(s - 1) * columns + columns * (end_segment - 1);
						if (z != columns - 1)
						{
							triangle.push_back(TopologyModule::Triangle(pt_len + 1, temp, temp + 1));	//���ɵ�������Ȧ

						}
						else
						{
							triangle.push_back(TopologyModule::Triangle(pt_len + 1, temp, temp - columns + 1));	//���ɵ�������Ȧ���һ����

						}

					}
				}

			}


			//�任

			Quat<Real> q = computeQuaternion();

			q.normalize();

			auto RV = [&](const Coord& v)->Coord {
				return center + q.rotate(v - center);
			};

			int numpt = vertices.size();

			for (int i = 0; i < numpt; i++)
			{
				//vertices[i][1] -= height / 2;
				vertices[i] = RV(vertices[i] * scale + RV(center));
			}



			if (this->varReverseNormal()->getData() == true)
			{
				int trinum = triangle.size();
				for (int i = 0; i < trinum; i++)
				{
					int temp;
					temp = triangle[i][0];
					triangle[i][0] = triangle[i][2];
					triangle[i][2] = temp;
				}
			}




			triangleSet->setPoints(vertices);
			triangleSet->setTriangles(triangle);

			triangleSet->update();

			vertices.clear();
			triangle.clear();
			printf("Turning done\n");
		}
	}






	DEFINE_CLASS(TurningModel);
}