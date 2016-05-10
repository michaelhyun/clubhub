class ClubsController < ApplicationController
layout "users"
  before_action :set_club, only: [ :edit, :update, :destroy]



  # GET /clubs
  # GET /clubs.json
  def index
    @clubs = Club.all
  end

  # GET /clubs/1
  # GET /clubs/1.json
  def show
<<<<<<< HEAD
    # @clubs = Club.where(category: :param)
    if (params[:param].eql? "Academics")
      @clubs = Club.where("category = 'Academics'")
    end 

    if (params[:param].eql? "Engineering")
      @clubs = Club.where("category = 'Engineering'")
    end 

    if (params[:param].eql? "Cultural")
      @clubs = Club.where("category = 'Cultural'")
    end 

    if (params[:param].eql? "Religious")
      @clubs = Club.where("category = 'Religious'")
    end 

    if (params[:param].eql? "Science")
      @clubs = Club.where("category = 'Science'")
    end 

    if (params[:param].eql? "Other")
      @clubs = Club.where("category = 'Other'")
    end

    if (params[:param].eql? "Service and Social Justice")
      @clubs = Club.where("category = 'Service and Social Justice'")
    end

    if (params[:param].eql? "Performing Arts")
      @clubs = Club.where("category = 'Performing Arts'")
    end

    if (params[:param].eql? "Greek")
      @clubs = Club.where("category = 'Greek'")
    end

    if (params[:param].eql? "Arts")
      @clubs = Club.where("category = 'Arts'")
    end

    if (params[:param].eql? "Health")
      @clubs = Club.where("category = 'Health'")
    end

    if (params[:param].eql? "Music")
      @clubs = Club.where("category = 'Music'")
    end

    if (params[:param].eql? "Politics")
      @clubs = Club.where("category = 'Politics'")
    end

    if (params[:param].eql? "Sports")
      @clubs = Club.where("category = 'Sports'")
    end

=======
    @club = Club.where(name: params[:param])
>>>>>>> origin/ka-branch
  end

  # GET /clubs/new
  def new
    @club = Club.new
  end

  # GET /clubs/1/edit
  def edit
  end

 


  # POST /clubs
  # POST /clubs.json
  def create
    @club = Club.new(club_params)

    respond_to do |format|
      if @club.save
        format.html { redirect_to @club, notice: 'Club was successfully created.' }
        format.json { render :show, status: :created, location: @club }
      else
        format.html { render :new }
        format.json { render json: @club.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /clubs/1
  # PATCH/PUT /clubs/1.json
  def update
    respond_to do |format|
      if @club.update(club_params)
        format.html { redirect_to @club, notice: 'Club was successfully updated.' }
        format.json { render :show, status: :ok, location: @club }
      else
        format.html { render :edit }
        format.json { render json: @club.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /clubs/1
  # DELETE /clubs/1.json
  def destroy
    @club = Club.find(params[:id])
    @club.destroy
    respond_to do |format|
      format.html { redirect_to clubs_url, notice: 'Club was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.

    def set_club
      @club = Club.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def club_params
      params.require(:club).permit(:Name, :admin, :numberOfMembers, :category)
    end
  end

