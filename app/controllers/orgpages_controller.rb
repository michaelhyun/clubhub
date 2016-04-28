class OrgpagesController < ApplicationController
	
  before_action :set_orgpage, only: [:show, :edit, :update, :destroy]

  # GET /posts
  # GET /posts.json
  def index
    @orgpages= Orgpage.all
  end

  # GET /posts/1
  # GET /posts/1.json
  def show
  end

  # GET /posts/new
  def new
    @orgpage = Orgpage.new
  end

  # GET /posts/1/edit
  def edit
  end

  # POST /posts
  # POST /posts.json
  def create
    @orgpage = Orgpage.new(orgpage_params)

    respond_to do |format|
      if @orgpage.save
        format.html { redirect_to @orgpage, notice: 'Org Page was successfully created.' }
        format.json { render :show, status: :created, location: @orgpage }
      else
        format.html { render :new }
        format.json { render json: @orgpage.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /posts/1
  # PATCH/PUT /posts/1.json
  def update
    respond_to do |format|
      if @orgpage.update(orgpage_params)
        format.html { redirect_to @orgpage, notice: 'Post was successfully updated.' }
        format.json { render :show, status: :ok, location: @orgpage }
      else
        format.html { render :edit }
        format.json { render json: @orgpage.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /posts/1
  # DELETE /posts/1.json
  def destroy
    @orgpage.destroy
    respond_to do |format|
      format.html { redirect_to posts_url, notice: 'Post was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_orgpage
      @orgpage = Orgpage.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def orgpage_params
      params.require(:orgpage).permit(:title, :body)
    end
end
