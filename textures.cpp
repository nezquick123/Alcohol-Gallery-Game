#include "textures.h"
#include <GL/glew.h>
#include "globals.h"
#include <glm/gtc/type_ptr.hpp>

void textureCube(glm::mat4 M, GLuint tex, glm::vec4 lp, bool inside) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Specify source of the data for the attribute vertex

	glUniform1i(sp->u("negate"), inside);
	glEnableVertexAttribArray(sp->a("color")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Specify source of the data for the attribute color
	glUniform4f(sp->u("lp"), lp.x, lp.y, lp.z, 1);
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal


	glEnableVertexAttribArray(sp->a("texCoord")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoords); //Specify source of the data for the attribute normal
	glUniform1i(sp->u("textureMap0"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, tex);

	/// //////////////////////////////////////

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Draw the object
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("color")); //Disable sending data to the attribute color
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
}

void textureCubeSpec(glm::mat4 M, GLuint tex, GLuint texSpec, glm::vec4 lp, bool inside) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Specify source of the data for the attribute vertex

	glUniform1i(sp->u("negate"), inside);
	glEnableVertexAttribArray(sp->a("color")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Specify source of the data for the attribute color
	glUniform4f(sp->u("lp"), lp.x, lp.y, lp.z, 1);
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal


	glEnableVertexAttribArray(sp->a("texCoord")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoords); //Specify source of the data for the attribute normal
	glUniform1i(sp->u("textureMap0"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, tex);

	glUniform1i(sp->u("textureMap1"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, texSpec);



	/// //////////////////////////////////////

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Draw the object
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("color")); //Disable sending data to the attribute color
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
}
