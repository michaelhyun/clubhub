class CategoriesController < ApplicationController
layout "users"
  before_action :set_category, only: []

  # GET /categories
  # GET /categories.json
  def index
    @categories = Category.all
  end

  # GET /categories/1
  # GET /categories/1.json


  # GET /categories/new


  # GET /categories/1/edit


  # POST /categories
  # POST /categories.json


  # PATCH/PUT /categories/1
  # PATCH/PUT /categories/1.json
  #def update
   # respond_to do |format|
    #  if @category.update(category_params)
     #   format.html { redirect_to @category, notice: 'Category was successfully updated.' }
      #  format.json { render :show, status: :ok, location: @category }
     # else
      #  format.html { render :edit }
      #  format.json { render json: @category.errors, status: :unprocessable_entity }
     # end
   # end
 # end

  # DELETE /categories/1
  # DELETE /categories/1.json
  #def destroy
   # @category.destroy
   # respond_to do |format|
   #   format.html { redirect_to categories_url, notice: 'Category was successfully destroyed.' }
   #   format.json { head :no_content }
   # end
 # end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_category
      @category = Category.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def category_params
      params.require(:category).permit(:genre)
    end
end
