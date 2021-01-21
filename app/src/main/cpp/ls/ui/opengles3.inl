


namespace ls{
    string OpenGlEs3::LOG_TAG = "GLES3";

    bool OpenGlEs3::checkGlError(const char* funcName) {
        GLint err = glGetError();
        if (err != GL_NO_ERROR) {
            stringstream msg;
            msg << "GL error after " <<  funcName << "(): " << err << endl;
            log->e(LOG_TAG, msg);
            return true;
        }
        return false;
    }

    GLuint OpenGlEs3::createShader(GLenum shaderType, const char* src) {
        GLuint shader = glCreateShader(shaderType);
        if (!shader) {
            checkGlError("glCreateShader");
            return 0;
        }
        glShaderSource(shader, 1, &src, NULL);

        GLint compiled = GL_FALSE;
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLogLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
            if (infoLogLen > 0) {
                GLchar* infoLog = (GLchar*)malloc(infoLogLen);
                if (infoLog) {
                    glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);

                    stringstream msg;
                    msg << "Could not compile "
                                    << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader:\n"
                                    <<  infoLog << endl;
                    log->e(LOG_TAG,msg);
                    free(infoLog);
                }
            }
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint OpenGlEs3::createProgram(const char* vtxSrc, const char* fragSrc) {
        GLuint vtxShader = 0;
        GLuint fragShader = 0;
        GLuint program = 0;
        GLint linked = GL_FALSE;

        vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
        if (!vtxShader)
            goto exit;

        fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
        if (!fragShader)
            goto exit;

        program = glCreateProgram();
        if (!program) {
            checkGlError("glCreateProgram");
            goto exit;
        }
        glAttachShader(program, vtxShader);
        glAttachShader(program, fragShader);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {

            stringstream msg;
            msg << "Could not link program" << endl;
            log->e(LOG_TAG,msg);
            GLint infoLogLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
            if (infoLogLen) {
                GLchar* infoLog = (GLchar*)malloc(infoLogLen);
                if (infoLog) {
                    glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);

                    stringstream msg;
                    msg << "Could not link program:\n" << infoLog << "\n" << endl;
                    log->e(LOG_TAG,msg);
                    free(infoLog);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }

        exit:
        glDeleteShader(vtxShader);
        glDeleteShader(fragShader);
        return program;
    }

    void OpenGlEs3::printGlString(const char* name, GLenum s) {
        const char* v = (const char*)glGetString(s);
        stringstream msg;
        msg << "GL" << name << ": " << v << "\n" << endl;
        log->i(LOG_TAG,msg);
    }

    void OpenGlEs3::init() {
        printGlString("Version", GL_VERSION);
        printGlString("Vendor", GL_VENDOR);
        printGlString("Renderer", GL_RENDERER);
        printGlString("Extensions", GL_EXTENSIONS);

        const char vShaderStr[] =
                "#version 300 es\n"
                "uniform mat4 u_mvpMatrix; // matrix to convert position from\n"
                "layout(location = 0) in vec4 a_position; // input position value\n"
                "layout(location = 1) in vec4 a_color; // input color\n"
                "out vec4 v_color;\n"
                "void main()\n"
                "{\n"
                " v_color = a_color;\n"
                " gl_Position = u_mvpMatrix * a_position;\n"
                "}";
        const char fShaderStr[] =
                "#version 300 es \n"
                "precision mediump float; \n"
                "in vec4 v_color; \n"
                "out vec4 o_fragColor; \n"
                "void main() \n"
                "{ \n"
                " o_fragColor = v_color; \n"
                "}" ;


        programObject = createProgram(vShaderStr, fShaderStr);
        if ( programObject == 0 ){
            return;
        }

        glClearColor ( 0.0f, 0.3f, 0.3f, 1.0f );
        glEnable(GL_DEPTH_TEST);

        mash.clear();

        mash.push_back(Vec3f(0.0f, 0.5f, -0.5f));
        mash.push_back(Vec3f(-0.5f, -0.5f, 0.0f));
        mash.push_back(Vec3f(0.5f, -0.5f, 0.5f));

        mash.push_back(Vec3f(0.0f, 0.3f, 0.0f));
        mash.push_back(Vec3f(-0.1f, -0.3f, 0.0f));
        mash.push_back(Vec3f(0.2f, -0.3f, 0.0f));


    }

    void OpenGlEs3::resize(int w, int h) {
        width = w;
        height = h;

        stringstream msg;
        msg <<"Viewport:" << width <<"/" << height << endl;
        log->i(LOG_TAG,msg);
        mPerspective = perspective(60.0f, float(width)/height, 0.0001f, 10000.0f);
        glViewport(0, 0, width, height);
    }

    bool OpenGlEs3::draw() {

        GLfloat color[4*6] = { 0.0f, 0.0f, 1.0f, 1.0f,
                               0.0f, 0.0f, 1.0f, 1.0f,
                               0.0f, 0.0f, 1.0f, 1.0f,

                               1.0f, 0.0f, 0.0f, 1.0f,
                               1.0f, 0.0f, 0.0f, 1.0f,
                               1.0f, 0.0f, 0.0f, 1.0f,};
        //

        mTransform = rotate(Mat4f(1),radians(0.0f),Vec3f(0,1,0));
        mTransform = translate(mTransform,Vec3f(0.000,0.0,-2));
        //mTransform = scale(mTransform,Vec3f(1.001,1.001,1.001));


        auto matrix = mPerspective*mTransform;

        // Clear the color buffer
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Use the program object
        glUseProgram ( programObject );
        // Load the vertex data

        glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*) mash.data() );
        glEnableVertexAttribArray ( 0 );
        glVertexAttribPointer ( 1, 4, GL_FLOAT, GL_FALSE, 0, color );
        glEnableVertexAttribArray ( 1 );
        GLint u_mvpMatrix = glGetUniformLocation ( programObject,"u_mvpMatrix" );
        glUniformMatrix4fv( u_mvpMatrix, 1, GL_FALSE, (GLfloat*)&matrix );

        glDrawArrays ( GL_TRIANGLES, 0, 3 );
        glDrawArrays ( GL_TRIANGLES, 3, 3 );


        glDisableVertexAttribArray ( 0 );
        glDisableVertexAttribArray ( 1 );
        return true;
    }
}

