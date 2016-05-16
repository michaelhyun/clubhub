class CategoriesController < ApplicationController
layout "users"


  # GET /categories
  # GET /categories.json
  def index
    @categories = Category.all
  end

  # GET /categories/1
  # GET /categories/1.json
  def show
    @categories = Category.where(genre: params[:param])
  end

  # GET /categories/new
def create 
     @category = Category.new(category_params)

    respond_to do |format|
      if @category.save
        format.html { redirect_to @category, notice: 'Category was successfully created.' }
        format.json { render :show, status: :created, location: @category }
      else
        format.html { render :new }
        format.json { render json: @category.errors, status: :unprocessable_entity }
      end
    end
end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_category
      @categories = Category.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def category_params
      params.require(:category).permit(:genre, :clubs)
    end
   end
