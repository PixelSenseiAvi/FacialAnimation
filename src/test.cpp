typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> MatrixXXf;
// typedef Eigen::Map<MatrixXXf> MatrixMap;
MatrixXXf* mat;
void test()
{
    mat = new MatrixXXf(3,3);
    //MatrixXXf vec(3,1);

    // MatrixXXf mat_2 = vec;

    //vec(0,0) = 5;
    //vec(1,0) = 6;
    //vec(2,0) = 7;

    *mat << 1,2,3,
            4,5,6,
            7,8,9;
    Eigen::VectorXf vec(3);
    // Eigen::Matrix<float,3,1>* m_x = &vec;

    *mat = mat->colwise() + vec;

    // mat->operator()(0,0) = 5.0f;
    // (*mat)(2,2) = 6.0f;
    std::cout << *mat << std::endl;
    std::cout << vec << std::endl;
    // std::cout << mat->size() << std::endl;
    // mat->conservativeResize(5,5);
    // std::cout << *mat << std::endl;
    // std::cout << mat->size() << std::endl;
    delete mat;

    // MatrixMap mat_map(mat->data(),2,2);
}

void loader_test()
{
    BlendshapeLoader loader;
    loader.load("../res/high-res/");
}

void BlendshapeMesh::test_neutral_face()
{
    std::vector<float> mesh_positions; //vertex positions
	std::vector<float> mesh_tex_coords;   //texture coordinates
	std::vector<float> mesh_normals;  //normals 
	std::vector<uint32_t> mesh_indices; //connectivity

    // if(!read_neutral(std::string("../res/high-res/neutral_new.obj"),mesh_positions,mesh_tex_coords,mesh_normals,mesh_indices))
    // {
    //     std::cout << "Error : Unable to read file !! \n" << std::endl;
    //     return;
    // }

    for(int idx=0;idx<mesh_normals.size()/3;idx++)
    {
        float normal_val = mesh_normals[idx];
        //printf("%f %f %f\n",normal_val);
    }

    glGenBuffers(1,&VBO);
    glGenBuffers(1,&IBO);
    glGenBuffers(1,&VNO);

    uint32_t vertex_count = mesh_positions.size()/3;
    icount = mesh_indices.size();

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,3 * vertex_count * sizeof(float),mesh_positions.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,VNO);
    glBufferData(GL_ARRAY_BUFFER,3 * vertex_count * sizeof(float),mesh_normals.data(),GL_STATIC_DRAW);
    /// Setup vertex array

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,VNO);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,icount * sizeof(uint32_t),mesh_indices.data(),GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}


void BlendshapeMesh::load_teapot()
{
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&IBO);
    glGenBuffers(1,&VNO);

    uint32_t vertex_count = teapot_vertex_count;
    
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,3 * vertex_count * sizeof(float),teapot_vertex_points,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,VNO);
    glBufferData(GL_ARRAY_BUFFER,3 * vertex_count * sizeof(float),teapot_normals,GL_STATIC_DRAW);
    /// Setup vertex array

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,VNO);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

	/// 
	int debug_stride = 30;
	for(int i =0;i<indices.size();i++)
	{
		uint32_t val = indices[i];
		std::cout << val << " ";
		if(i%debug_stride == 0)
			std::cout << std::endl;
	}

	std::cout << "====================" << std::endl;



        int count = loader->final_shape->rows()/3;
    float* points = loader->final_shape->data();
    float zdist = std::numeric_limits<float>::max();
    int index=-1;
    float min_dist = std::numeric_limits<float>::max();
    
    for(int i=0;i<count;i++)
    {
        int pindex = i*3;
        float p_x = points[pindex];
        float p_y = points[pindex+1];
        float p_z = points[pindex+2];

        float pc_x = p_x - ray.position.x;
        float pc_y = p_y - ray.position.y;
        float pc_z = p_z - ray.position.z;

        float dot = ray.direction.x * pc_x + ray.direction.y * pc_y +
                    ray.direction.z * pc_z;
        if(dot <= 0)
            continue;
        
        float pr_x = dot*ray.direction.x;
        float pr_y = dot*ray.direction.y;
        float pr_z = dot*ray.direction.z;

        float temp1 = p_x*pr_x;
        float temp2 = p_y*pr_y;
        float temp3 = p_z*pr_z;

        float sqr_dist = (temp1*temp1) + (temp2*temp2) + (temp3*temp3);
        
        if(sqr_dist < min_dist)
            min_dist = sqr_dist;
        
        if(sqr_dist > 100.0f)
            continue;
        
        index = i;

        if(pr_z < zdist)
        {
            index = i;
            zdist = pr_z;
        }
    }

    if(index != -1)
    {
        std::cout << "vertex index : " << index << std::endl;
    }  