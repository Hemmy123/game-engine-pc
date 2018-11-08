#include "Shadows.h"
#include "FilePaths.h"

Shadows::Shadows(Renderer * parentRenderer):
	m_parentRenderer(parentRenderer)
{
	
	m_sceneShader = new Shader(
		SHADERVERTDIR"shadowScene_vert.glsl",
		SHADERFRAGDIR"shadowScene_frag.glsl");

	m_shadowShader = new Shader(
		SHADERVERTDIR"shadow_vert.glsl",
		SHADERFRAGDIR"shadow_frag.glsl");


	Vector3 pos(100, 400, 100);
	Vector4 col(1, 1, 1, 1);
	float radius	= 6000.0f;
	m_light			= new Light(pos, col, radius);

	// TODO: Check this!
	m_biasMatrix = Matrix4(
		Matrix4::Scale(Vector3(0.5, 0.5, 0.5)) *
		Matrix4::Translation(Vector3(0.5,0.5,0.5))
	);

	generateFBO();
}	

Shadows::~Shadows()
{
	glDeleteTextures(1, &m_shadowTex);
	glDeleteFramebuffers(1, &m_shadowFBO);

	delete m_light;
	delete m_shadowShader;
	delete m_sceneShader;
}

void Shadows::drawShadowScene(){
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);

	// Clear depth buffer since we are about to draw to it.
	glClear(GL_DEPTH_BUFFER_BIT);

	// Sets the view port to be the origin?
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	// Don't draw any colours! (R,G,B,A)
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	m_parentRenderer->setCurrentShader(m_shadowShader);

	m_parentRenderer->checkErrors();

	Matrix4 lightViewMatrix = Matrix4::BuildViewMatrix(
		m_light->getPosition(), Vector3(0, 0, 0));


	Matrix4 lightTextureMatrix =
		m_biasMatrix *
		(m_parentRenderer->getProjectionMatrix() * m_parentRenderer->getViewMatrix());



	m_parentRenderer->setViewMatrix(lightViewMatrix);
	m_parentRenderer->setTextureMatrix(lightTextureMatrix);

	m_parentRenderer->updateShaderMatrices();
	m_parentRenderer->checkErrors();


	// ?
	m_parentRenderer->drawAllMeshes();

	m_parentRenderer->checkErrors();


	// back to default settings
	glUseProgram(0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, m_parentRenderer->getWidth(), m_parentRenderer->getHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Shadows::drawCombinedScene(GLuint sceneFbo){

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);

	m_parentRenderer->setCurrentShader(m_sceneShader);
	GLuint diffuseTexLoc	= glGetUniformLocation(m_sceneShader->getProgram(), "diffuseTex");
	GLuint bumpTexLoc		= glGetUniformLocation(m_sceneShader->getProgram(), "bumpTex");
	GLuint shadowTexLoc		= glGetUniformLocation(m_sceneShader->getProgram(), "shadowTex");
	GLuint cameraLoc		= glGetUniformLocation(m_sceneShader->getProgram(), "cameraPos");


	glUniform1i(diffuseTexLoc, 0);
	m_parentRenderer->checkErrors();

	glUniform1i(bumpTexLoc, 1);
	m_parentRenderer->checkErrors();

	glUniform1i(shadowTexLoc, 2);
	m_parentRenderer->checkErrors();

	Vector3 cameraPos = m_parentRenderer->getCamera()->GetPosition();
	glUniform3fv(cameraLoc, 1, (float*)&cameraPos);
	m_parentRenderer->checkErrors();


	m_parentRenderer->setShaderLight(m_sceneShader, *m_light);
	m_parentRenderer->checkErrors();


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_shadowTex);

	Matrix4 viewMatrix = m_parentRenderer->getCamera()->BuildViewMatrix();
	m_parentRenderer->setViewMatrix(viewMatrix);

	// Has to use somesort of raw draw method
	m_parentRenderer->drawAllRenderObjects();
	
	
	m_parentRenderer->checkErrors();

	glUseProgram(0);

}

void Shadows::drawScene(GLuint sceneFBO)
{
	// clear buffers?

	drawShadowScene();
	drawCombinedScene(sceneFBO);
}

int Shadows::generateFBO()
{
	glGenTextures(1, &m_shadowTex);

	// Generate Texture
	glBindTexture(GL_TEXTURE_2D, m_shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
	

	// Generate FBO

	glGenFramebuffers(1, &m_shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
	// Creating depth attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowTex, 0);
	// Which colour buffer to draw to when draw is called.
	// GL_NONE because we only want the depth texture.
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return 0;
}
